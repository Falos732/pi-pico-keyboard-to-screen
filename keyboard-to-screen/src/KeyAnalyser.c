#include <stdio.h>
#include <string.h>
#include "pico/stdlib.h"
#include "pio_usb.h"
#include "tusb.h"

// Configuration USB Host sur GPIO
#define HOST_PIN_DP   2      // D+ sur GP2
#define LED_PIN       25     // LED intégrée

// Structure pour logger les événements clavier
typedef struct {
    uint32_t timestamp_ms;
    uint8_t modifier;
    uint8_t keycodes[6];
} keyboard_report_t;

#define MAX_REPORTS 200
static keyboard_report_t reports[MAX_REPORTS];
static uint32_t report_count = 0;

// État précédent pour détecter les changements
static uint8_t prev_report[8] = {0};

// Table de conversion keycode USB HID vers ASCII
const char keycode_to_ascii_table[128] = {
    0,   0,   0,   0,   'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l',
    'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z', '1', '2',
    '3', '4', '5', '6', '7', '8', '9', '0', '\n', 0,  '\b', '\t', ' ', '-', '=', '[',
    ']', '\\', 0,   ';', '\'', '`', ',', '.', '/',  0,   0,   0,   0,   0,   0,   0,
    0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
    0,   0,   0,   0,   '/', '*', '-', '+', '\n', '1', '2', '3', '4', '5', '6', '7',
    '8', '9', '0', '.', 0,   0,   0,   '=', 0,   0,   0,   0,   0,   0,   0,   0,
    0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0
};

// Conversion avec shift
char keycode_to_char(uint8_t keycode, bool shift) {
    if (keycode >= 0x04 && keycode <= 0x1D) { // a-z
        char c = 'a' + (keycode - 0x04);
        return shift ? (c - 32) : c;
    }
    
    if (!shift) {
        return keycode_to_ascii_table[keycode];
    }
    
    // Caractères modifiés par Shift
    switch (keycode) {
        case 0x1E: return '!';
        case 0x1F: return '@';
        case 0x20: return '#';
        case 0x21: return '$';
        case 0x22: return '%';
        case 0x23: return '^';
        case 0x24: return '&';
        case 0x25: return '*';
        case 0x26: return '(';
        case 0x27: return ')';
        case 0x2D: return '_';
        case 0x2E: return '+';
        case 0x2F: return '{';
        case 0x30: return '}';
        case 0x31: return '|';
        case 0x33: return ':';
        case 0x34: return '"';
        case 0x36: return '<';
        case 0x37: return '>';
        case 0x38: return '?';
        default: return keycode_to_ascii_table[keycode];
    }
}

// Callback: Clavier détecté
void tuh_hid_mount_cb(uint8_t dev_addr, uint8_t instance, uint8_t const* desc_report, uint16_t desc_len) {
    (void)desc_report;
    (void)desc_len;
    
    uint16_t vid, pid;
    tuh_vid_pid_get(dev_addr, &vid, &pid);
    
    printf("\n╔════════════════════════════════════╗\n");
    printf("║   CLAVIER USB DÉTECTÉ !            ║\n");
    printf("╚════════════════════════════════════╝\n");
    printf("Device Address: %d\n", dev_addr);
    printf("Instance: %d\n", instance);
    printf("VID:PID = %04X:%04X\n\n", vid, pid);
    
    // Activer LED
    gpio_put(LED_PIN, 1);
    
    // Demander les rapports
    if (!tuh_hid_receive_report(dev_addr, instance)) {
        printf("ERREUR: Impossible de démarrer la réception\n");
    } else {
        printf("✓ Réception des rapports activée\n");
        printf("Tapez sur le clavier pour voir les événements...\n\n");
    }
}

// Callback: Clavier déconnecté
void tuh_hid_umount_cb(uint8_t dev_addr, uint8_t instance) {
    printf("\n╔════════════════════════════════════╗\n");
    printf("║   CLAVIER DÉCONNECTÉ               ║\n");
    printf("╚════════════════════════════════════╝\n");
    printf("Device: %d, Instance: %d\n\n", dev_addr, instance);
    
    gpio_put(LED_PIN, 0);
}

// Callback: Rapport HID reçu
void tuh_hid_report_received_cb(uint8_t dev_addr, uint8_t instance, uint8_t const* report, uint16_t len) {
    if (len < 8) {
        tuh_hid_receive_report(dev_addr, instance);
        return;
    }
    
    // Vérifier si le rapport a changé
    if (memcmp(prev_report, report, 8) != 0) {
        uint32_t timestamp = to_ms_since_boot(get_absolute_time());
        uint8_t modifier = report[0];
        
        // Stocker dans le buffer
        if (report_count < MAX_REPORTS) {
            reports[report_count].timestamp_ms = timestamp;
            reports[report_count].modifier = modifier;
            memcpy(reports[report_count].keycodes, &report[2], 6);
            report_count++;
        }
        
        // Affichage temps réel
        printf("[%6lu ms] ", timestamp);
        
        // Afficher modifiers
        if (modifier) {
            if (modifier & 0x01) printf("LCtrl ");
            if (modifier & 0x02) printf("LShift ");
            if (modifier & 0x04) printf("LAlt ");
            if (modifier & 0x08) printf("LGUI ");
            if (modifier & 0x10) printf("RCtrl ");
            if (modifier & 0x20) printf("RShift ");
            if (modifier & 0x40) printf("RAlt ");
            if (modifier & 0x80) printf("RGUI ");
        }
        
        // Afficher les touches pressées
        bool shift = (modifier & 0x22) != 0;
        bool has_keys = false;
        
        printf("Keys: ");
        for (int i = 2; i < 8; i++) {
            if (report[i] != 0) {
                has_keys = true;
                char c = keycode_to_char(report[i], shift);
                printf("[0x%02X='%c'] ", report[i], (c >= 32 && c < 127) ? c : '?');
            }
        }
        
        if (!has_keys && !modifier) {
            printf("(released)");
        }
        
        printf("\n");
        
        // Sauvegarder l'état
        memcpy(prev_report, report, 8);
    }
    
    // Demander le prochain rapport
    tuh_hid_receive_report(dev_addr, instance);
}

// Afficher un résumé des captures
void print_summary() {
    printf("\n\n");
    printf("╔════════════════════════════════════════════════╗\n");
    printf("║       RÉSUMÉ DES ÉVÉNEMENTS CAPTURÉS          ║\n");
    printf("╚════════════════════════════════════════════════╝\n");
    printf("Total d'événements:%u\n\n", (unsigned)report_count);
    
    // Reconstruire le texte tapé
    printf("Texte reconstitué:\n");
    printf("─────────────────\n");
    
    for (uint32_t i = 0; i < report_count; i++) {
        bool shift = (reports[i].modifier& (KEYBOARD_MODIFIER_LEFTSHIFT | KEYBOARD_MODIFIER_RIGHTSHIFT));
        
        for (int j = 0; j < 6; j++) {
            if (reports[i].keycodes[j] != 0) {
                char c = keycode_to_char(reports[i].keycodes[j], shift);
                if (c == '\n') {
                    printf("\n");
                } else if (c == '\b') {
                    printf("[BACKSPACE]");
                } else if (c >= 32 && c < 127) {
                    printf("%c", c);
                }
            }
        }
    }
    printf("\n\n");
}

int main() {
    // Initialisation UART pour debug (car USB sera utilisé pour Host)
    stdio_init_all();
    sleep_ms(2000);
    
    printf("\n");
    printf("╔════════════════════════════════════════════════╗\n");
    printf("║   RASPBERRY PI PICO - USB HOST KEYBOARD       ║\n");
    printf("║            via GPIO (PIO-USB)                  ║\n");
    printf("╚════════════════════════════════════════════════╝\n");
    printf("\n");
    
    // Configuration LED
    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT);
    gpio_put(LED_PIN, 0);
    
    // Configuration PIO-USB pour Host
    pio_usb_configuration_t pio_cfg = PIO_USB_DEFAULT_CONFIG;
    pio_cfg.pin_dp = HOST_PIN_DP;
    
    printf("Configuration USB Host:\n");
    printf("  - D+ Pin: GP%d\n", HOST_PIN_DP);
    printf("  - D- Pin: GP%d (automatique)\n", HOST_PIN_DP + 1);
    printf("  - Mode: PIO USB Host\n");
    printf("\n");
    
    // Initialiser TinyUSB Host avec PIO
    tuh_configure(BOARD_TUH_RHPORT, TUH_CFGID_RPI_PIO_USB_CONFIGURATION, &pio_cfg);
    tuh_init(BOARD_TUH_RHPORT);
    
    printf("✓ USB Host initialisé\n");
    printf("✓ Branchez un clavier USB sur les GPIO...\n");
    printf("\n");
    printf("Attendez la détection (LED s'allumera)...\n");
    printf("═══════════════════════════════════════════════\n\n");
    
    uint32_t last_summary = 0;
    
    // Boucle principale
    while (true) {
        static uint32_t last_frame = 0;
        uint32_t now = to_ms_since_boot(get_absolute_time());
        if (now - last_frame >= 1) {
            pio_usb_host_frame();
            last_frame = now;
        }
        // Traiter les événements USB
        tuh_task();
        
        // Afficher un résumé toutes les 30 secondes si des événements existent
        if (report_count > 0 && (now - last_summary) > 30000) {
            print_summary();
            last_summary = now;
        }
        
        tight_loop_contents();
    }
    
    return 0;
}