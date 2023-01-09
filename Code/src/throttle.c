#include <stdlib.h>

#include <pico/stdlib.h>
#include <hardware/gpio.h>
#include <hardware/adc.h>
#include "bsp/board.h"
#include <tusb.h>
#include "usb_descriptors.h"
#include "usb_tool.h"

#include <pico/stdio.h>

const uint16_t THROTTLE_MIN = 0, THROTTLE_MAX = 69;

struct ButtonStates
{
    // top buttons from top, going clockwise
};

struct
{
    int potentiometer;
} inputPins = {
    .potentiometer = 26,
};

void initInputPins(void);

void readThrottle(int16_t *throttle);
void readButtonStates(struct ButtonStates *out);
void calibrateThrottle(void);

// send to computer
void hidTask(void);
void sendGamepadReport(int16_t *throttle, struct ButtonStates *buttons);

void led_on(void) { gpio_put(PICO_DEFAULT_LED_PIN, 1); }
void led_off(void) { gpio_put(PICO_DEFAULT_LED_PIN, 0); }

int mmain()
{

    gpio_init(PICO_DEFAULT_LED_PIN);
    gpio_set_dir(PICO_DEFAULT_LED_PIN, GPIO_OUT);

    gpio_init(26);
    gpio_set_dir(26, GPIO_IN);

    const float conversion = 3.3f / (1 << 12);

    while (1)
    {
        if (gpio_get(26))
            led_on();
        else
            led_off();
        sleep_ms(10);
    }
}

int main()
{
    board_init();
    tusb_init();

    adc_init();

    gpio_init(PICO_DEFAULT_LED_PIN);
    gpio_set_dir(PICO_DEFAULT_LED_PIN, GPIO_OUT);

    initInputPins();

    led_on();

    // calibrateJoystick();

    while (true)
    {

        bool toggle = board_button_read();

        // turn on led if button pressed
        if (toggle)
        {
            led_off();
        }
        tud_task();
        hidTask();
    }
}

void sendGamepadReport(int16_t *throttle, struct ButtonStates *buttons)
{
    if (!tud_hid_ready())
        return;

    // generate button report
    uint16_t button_report = 0;

    // dpad

    hid_gamepad_report_t gamepad_report = {
        0,             // x stick
        0,             // y stick
        *throttle,     // z?
        0,             // x right stick
        0,             // y
        0,             // z
        0,             // hat
        button_report, // buttons
    };

    tud_hid_report(REPORT_ID_GAMEPAD, &gamepad_report, sizeof(gamepad_report));
}

bool isButtonsActive(struct ButtonStates *states) { return 0; }
void hidTask(void)
{
    // only send reports every 10 ms
    const uint32_t interval_ms = 10;
    static uint32_t start_ms   = 0;
    if (board_millis() - start_ms < interval_ms)
        return;
    start_ms += interval_ms;

    int16_t throttle = 0;
    readThrottle(&throttle);
    struct ButtonStates buttonStates;
    readButtonStates(&buttonStates);

    bool isInput = throttle != 0 || isButtonsActive(&buttonStates);
    if (isInput)
        led_off();
    else
        led_on();

    if (getUsbState() == USB_IDLE && isInput)
        tud_remote_wakeup();
    else
        sendGamepadReport(&throttle, &buttonStates);
}

int16_t convertRawThrottle(uint16_t raw)
{
    const float percentOfMax =
        (float)(THROTTLE_MAX - THROTTLE_MIN) /
        (1 << 8); // percent of potentiometer max vs actual max
    float calibratedValue = (float)raw / percentOfMax;
    int16_t finalValue    = (int16_t)calibratedValue - INT16_MAX - THROTTLE_MIN;

    return ~(int16_t)((raw >> 4) / percentOfMax - 128); // convert to 8-bit
}

// read joystick input
void readThrottle(int16_t *out)
{
    adc_select_input(abs(26 - inputPins.potentiometer));
    uint16_t raw = adc_read();
    *out         = convertRawThrottle(raw);
}

// read button input
void readButtonStates(struct ButtonStates *out)
{
    *out = (struct ButtonStates){};
}

void initBtnPin(unsigned pin)
{
    gpio_init(pin);
    gpio_set_dir(pin, GPIO_IN);
    gpio_pull_down(pin);
}

void initInputPins(void) { adc_gpio_init(inputPins.potentiometer); }

// tusb callbacks

void tud_hid_report_complete_cb(
    uint8_t instance, uint8_t const *report, uint8_t len)
{
    (void)instance;
    (void)len;

    uint8_t next_report_id = report[0] + 1;

    if (next_report_id == REPORT_ID_GAMEPAD)
    {
        int16_t throttle;
        readThrottle(&throttle);
        struct ButtonStates buttonStates;
        readButtonStates(&buttonStates);
        sendGamepadReport(&throttle, &buttonStates);
    }
}

uint16_t tud_hid_get_report_cb(
    uint8_t instance,
    uint8_t report_id,
    hid_report_type_t report_type,
    uint8_t *buffer,
    uint16_t reqlen)
{
    // TODO not Implemented
    (void)instance;
    (void)report_id;
    (void)report_type;
    (void)buffer;
    (void)reqlen;

    return 0;
}

void tud_hid_set_report_cb(
    uint8_t instance,
    uint8_t report_id,
    hid_report_type_t report_type,
    uint8_t const *buffer,
    uint16_t bufsize)
{
    (void)instance;

    // ignore input
}
