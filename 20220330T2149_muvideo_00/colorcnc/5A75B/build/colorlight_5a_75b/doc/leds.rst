LEDS
====

Register Listing for LEDS
-------------------------

+------------------------------------------+-------------------------------------+
| Register                                 | Address                             |
+==========================================+=====================================+
| :ref:`LEDS_OUT <LEDS_OUT>`               | :ref:`0x00002000 <LEDS_OUT>`        |
+------------------------------------------+-------------------------------------+
| :ref:`LEDS_PWM_ENABLE <LEDS_PWM_ENABLE>` | :ref:`0x00002004 <LEDS_PWM_ENABLE>` |
+------------------------------------------+-------------------------------------+
| :ref:`LEDS_PWM_WIDTH <LEDS_PWM_WIDTH>`   | :ref:`0x00002008 <LEDS_PWM_WIDTH>`  |
+------------------------------------------+-------------------------------------+
| :ref:`LEDS_PWM_PERIOD <LEDS_PWM_PERIOD>` | :ref:`0x0000200c <LEDS_PWM_PERIOD>` |
+------------------------------------------+-------------------------------------+

LEDS_OUT
^^^^^^^^

`Address: 0x00002000 + 0x0 = 0x00002000`

    Led Output(s) Control.

    .. wavedrom::
        :caption: LEDS_OUT

        {
            "reg": [
                {"name": "out", "bits": 1},
                {"bits": 31},
            ], "config": {"hspace": 400, "bits": 32, "lanes": 4 }, "options": {"hspace": 400, "bits": 32, "lanes": 4}
        }


LEDS_PWM_ENABLE
^^^^^^^^^^^^^^^

`Address: 0x00002000 + 0x4 = 0x00002004`

    PWM Enable.

    Write ``1`` to enable PWM.

    .. wavedrom::
        :caption: LEDS_PWM_ENABLE

        {
            "reg": [
                {"name": "pwm_enable", "attr": 'reset: 1', "bits": 1},
                {"bits": 31},
            ], "config": {"hspace": 400, "bits": 32, "lanes": 4 }, "options": {"hspace": 400, "bits": 32, "lanes": 4}
        }


LEDS_PWM_WIDTH
^^^^^^^^^^^^^^

`Address: 0x00002000 + 0x8 = 0x00002008`

    PWM Width.

    Defines the *Duty cycle* of the PWM. PWM is active high for *Width* ``sys_clk``
    cycles and active low for *Period - Width* ``sys_clk`` cycles.

    .. wavedrom::
        :caption: LEDS_PWM_WIDTH

        {
            "reg": [
                {"name": "pwm_width[31:0]", "attr": 'reset: 512', "bits": 32}
            ], "config": {"hspace": 400, "bits": 32, "lanes": 1 }, "options": {"hspace": 400, "bits": 32, "lanes": 1}
        }


LEDS_PWM_PERIOD
^^^^^^^^^^^^^^^

`Address: 0x00002000 + 0xc = 0x0000200c`

    PWM Period.

    Defines the *Period* of the PWM in ``sys_clk`` cycles.

    .. wavedrom::
        :caption: LEDS_PWM_PERIOD

        {
            "reg": [
                {"name": "pwm_period[31:0]", "attr": 'reset: 1024', "bits": 32}
            ], "config": {"hspace": 400, "bits": 32, "lanes": 1 }, "options": {"hspace": 400, "bits": 32, "lanes": 1}
        }


