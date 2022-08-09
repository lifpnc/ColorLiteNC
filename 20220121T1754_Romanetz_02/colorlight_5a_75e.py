#!/usr/bin/env python3

#
# This file is part of LiteEth.
#
# Copyright (c) 2020 Florent Kermarrec <florent@enjoy-digital.fr>
# Copyright (c) 2022 Roman Pechenko <romanetz4@gmail.com>
# SPDX-License-Identifier: BSD-2-Clause

import os
import argparse
import platform

from migen import *

from litex_boards.platforms import colorlight_5a_75e
from litex_boards.targets.colorlight_5a_75x import _CRG

from litex.soc.cores.clock import *
from litex.soc.cores.pwm import PWM
from litex.soc.interconnect.csr import *
from litex.soc.integration.soc_core import *
from litex.soc.integration.builder import *
from litex.soc.cores.spi_flash import ECP5SPIFlash
from litex.soc.cores.gpio import GPIOOut,GPIOIn
from litex.soc.cores.led import LedChaser

from liteeth.phy.ecp5rgmii import LiteEthPHYRGMII
from litex.build.generic_platform import *
from litex.soc.interconnect.csr import *
from liteeth.frontend.stream import *
from liteeth.core import LiteEthUDPIPCore
from liteeth.common import *

# Bench SoC ----------------------------------------------------------------------------------------

encoders=([
    ("encoderJ9_1", 0,
     Subsignal("A", Pins("j9:0")), # J9.1
     Subsignal("B", Pins("j9:1")), # J9.2
     Subsignal("Z", Pins("j9:2")), # J9.3
     IOStandard("LVCMOS33")),
    ("encoderJ9_2", 1,
     Subsignal("A", Pins("j9:4")),  # J9.5
     Subsignal("B", Pins("j9:5")),  # J9.6
     Subsignal("Z", Pins("j9:6")),  # J9.7
     IOStandard("LVCMOS33")
     ),
    ("encoderJ10_1", 2,
     Subsignal("A", Pins("j10:0")),  # J10.1
     Subsignal("B", Pins("j10:1")),  # J10.2
     Subsignal("Z", Pins("j10:2")),  # J10.3
     IOStandard("LVCMOS33")
     ),
    ("encoderJ10_2", 3,
     Subsignal("A", Pins("j10:4")),  # J10.5
     Subsignal("B", Pins("j10:5")),  # J10.6
     Subsignal("Z", Pins("j10:6")),  # J10.7
     IOStandard("LVCMOS33")
     ),
    ("encoderJ11_1", 4,
     Subsignal("A", Pins("j11:0")),  # J11.1
     Subsignal("B", Pins("j11:1")),  # J11.2
     Subsignal("Z", Pins("j11:2")),  # J11.3
     IOStandard("LVCMOS33")
     ),
    ("encoderJ11_2", 5,
     Subsignal("A", Pins("j11:4")),  # J11.5
     Subsignal("B", Pins("j11:5")),  # J11.6
     Subsignal("Z", Pins("j11:6")),  # J11.7
     IOStandard("LVCMOS33")
     ),
])

stepgens=([
    ("stepgenJ1", 0,
     Subsignal("step", Pins("j1:0")), # J1.1
     Subsignal("dir", Pins("j1:1")), # J1.2
     IOStandard("LVCMOS33")),
    ("stepgenJ2", 1,
     Subsignal("step", Pins("j1:2")),  # J1.3
     Subsignal("dir", Pins("j1:4")),  # J1.5
     IOStandard("LVCMOS33")
     ),
    ("stepgenJ3", 2,
     Subsignal("step", Pins("j1:5")),  # J1.6
     Subsignal("dir", Pins("j1:6")),  # J1.7
     IOStandard("LVCMOS33")
     ),
    ("stepgenJ4", 3,
     Subsignal("step", Pins("j2:0")),  # J2.1
     Subsignal("dir", Pins("j2:1")),  # J2.2
     IOStandard("LVCMOS33")
     ),
    ("stepgenJ5", 4,
     Subsignal("step", Pins("j2:2")),  # J2.3
     Subsignal("dir", Pins("j2:4")),  # J2.5
     IOStandard("LVCMOS33")
     ),
    ("stepgenJ6", 5,
     Subsignal("step", Pins("j2:5")),  # J2.6
     Subsignal("dir", Pins("j2:6")),  # J2.7
     IOStandard("LVCMOS33")
     ),
])

_gpios_in = [
    ("gpio_in", 0, Pins("j12:0"), IOStandard("LVCMOS33")),
    ("gpio_in", 1, Pins("j12:1"), IOStandard("LVCMOS33")),
    ("gpio_in", 2, Pins("j12:2"), IOStandard("LVCMOS33")),
    ("gpio_in", 3, Pins("j12:4"), IOStandard("LVCMOS33")),
    ("gpio_in", 4, Pins("j12:5"), IOStandard("LVCMOS33")),
    ("gpio_in", 5, Pins("j12:6"), IOStandard("LVCMOS33")),
    ("gpio_in", 6, Pins("j13:0"), IOStandard("LVCMOS33")),
    ("gpio_in", 7, Pins("j13:1"), IOStandard("LVCMOS33")),
    ("gpio_in", 8, Pins("j13:2"), IOStandard("LVCMOS33")),
    ("gpio_in", 9, Pins("j13:4"), IOStandard("LVCMOS33")),
    ("gpio_in", 10, Pins("j13:5"), IOStandard("LVCMOS33")),
    ("gpio_in", 11, Pins("j13:6"), IOStandard("LVCMOS33")),
    ("gpio_in", 12, Pins("j14:0"), IOStandard("LVCMOS33")),
    ("gpio_in", 13, Pins("j14:1"), IOStandard("LVCMOS33")),
    ("gpio_in", 14, Pins("j14:2"), IOStandard("LVCMOS33")),
    ("gpio_in", 15, Pins("j14:4"), IOStandard("LVCMOS33")),
    ("gpio_in", 16, Pins("j14:5"), IOStandard("LVCMOS33")),
    ("gpio_in", 17, Pins("j14:6"), IOStandard("LVCMOS33")),
    ("gpio_in", 18, Pins("j15:0"), IOStandard("LVCMOS33")),
    ("gpio_in", 19, Pins("j15:1"), IOStandard("LVCMOS33")),
    ("gpio_in", 20, Pins("j15:2"), IOStandard("LVCMOS33")),
    ("gpio_in", 21, Pins("j15:4"), IOStandard("LVCMOS33")),
    ("gpio_in", 22, Pins("j15:5"), IOStandard("LVCMOS33")),
    ("gpio_in", 23, Pins("j15:6"), IOStandard("LVCMOS33")),
    ("gpio_in", 24, Pins("j16:0"), IOStandard("LVCMOS33")),
    ("gpio_in", 25, Pins("j16:1"), IOStandard("LVCMOS33")),
    ("gpio_in", 26, Pins("j16:2"), IOStandard("LVCMOS33")),
    ("gpio_in", 27, Pins("j16:4"), IOStandard("LVCMOS33")),
    ("gpio_in", 28, Pins("j16:5"), IOStandard("LVCMOS33")),
    ("gpio_in", 29, Pins("j16:6"), IOStandard("LVCMOS33"))
    ]
#gpios out
_gpios_out = [ \
    ("gpio_out", 0, Pins("j3:0"), IOStandard("LVCMOS33")),
    ("gpio_out", 1, Pins("j3:1"), IOStandard("LVCMOS33")),
    ("gpio_out", 2, Pins("j3:2"), IOStandard("LVCMOS33")),
    ("gpio_out", 3, Pins("j3:4"), IOStandard("LVCMOS33")),
    ("gpio_out", 4, Pins("j3:5"), IOStandard("LVCMOS33")),
    ("gpio_out", 5, Pins("j3:6"), IOStandard("LVCMOS33")),
    ("gpio_out", 6, Pins("j4:0"), IOStandard("LVCMOS33")),
    ("gpio_out", 7, Pins("j4:1"), IOStandard("LVCMOS33")),
    ("gpio_out", 8, Pins("j4:2"), IOStandard("LVCMOS33")),
    ("gpio_out", 9, Pins("j4:4"), IOStandard("LVCMOS33")),
    ("gpio_out", 10, Pins("j4:5"), IOStandard("LVCMOS33")),
    ("gpio_out", 11, Pins("j4:6"), IOStandard("LVCMOS33")),
    ("gpio_out", 12, Pins("j5:0"), IOStandard("LVCMOS33")),
    ("gpio_out", 13, Pins("j5:1"), IOStandard("LVCMOS33")),
    ("gpio_out", 14, Pins("j5:2"), IOStandard("LVCMOS33")),
    ("gpio_out", 15, Pins("j5:4"), IOStandard("LVCMOS33")),
    ("gpio_out", 16, Pins("j5:5"), IOStandard("LVCMOS33")),
    ("gpio_out", 17, Pins("j5:6"), IOStandard("LVCMOS33")),
    ("gpio_out", 18, Pins("j6:0"), IOStandard("LVCMOS33")),
    ("gpio_out", 19, Pins("j6:1"), IOStandard("LVCMOS33")),
    ("gpio_out", 20, Pins("j6:2"), IOStandard("LVCMOS33")),
    ("gpio_out", 21, Pins("j6:4"), IOStandard("LVCMOS33")),
    ("gpio_out", 22, Pins("j6:5"), IOStandard("LVCMOS33")),
    ("gpio_out", 23, Pins("j6:6"), IOStandard("LVCMOS33")), 
    ("gpio_out", 24, Pins("j7:0"), IOStandard("LVCMOS33")),
    ("gpio_out", 25, Pins("j7:1"), IOStandard("LVCMOS33")),
    ("gpio_out", 26, Pins("j7:2"), IOStandard("LVCMOS33")),
    ("gpio_out", 27, Pins("j7:4"), IOStandard("LVCMOS33")),
    ("gpio_out", 28, Pins("j7:5"), IOStandard("LVCMOS33")),
    ("gpio_out", 29, Pins("j7:6"), IOStandard("LVCMOS33")),
    ("gpio_out", 30, Pins("j8:0"), IOStandard("LVCMOS33")),
    ("gpio_out", 31, Pins("j8:1"), IOStandard("LVCMOS33")),
]

_en_out = [ \
    ("en_out", 0, Pins("j1:8"), IOStandard("LVCMOS33"))
]

_pwm_out = [ \
    ("pwm_out", 0, Pins("j1:11"), IOStandard("LVCMOS33")),
    ("pwm_out", 1, Pins("j1:12"), IOStandard("LVCMOS33")),
    ("pwm_out", 2, Pins("j1:13"), IOStandard("LVCMOS33")),
    ("pwm_out", 3, Pins("j8:2"), IOStandard("LVCMOS33")),
    ("pwm_out", 4, Pins("j8:4"), IOStandard("LVCMOS33")),
    ("pwm_out", 5, Pins("j8:5"), IOStandard("LVCMOS33")),
]

W = 32
F = 30
T = 10

class QuadEnc(Module,AutoCSR):
    def __init__(self, pads):
        self.pads = pads
        W = 16
        #self._out=CSRStorage(size=2*W, description="Encoders", write_from_dev=True)
        A = Signal(1)
        B = Signal(1)
        Z = Signal(1)
        zr = Signal(1)
        c = Signal(W - 1)
        i = Signal(W - 1)
        zl = Signal(1)
        self.out = Signal(2 * W)
        Ad = Signal(3)
        Bd = Signal(3)
        Zc = Signal(3)
        good_zero = Signal(1)
        good_one = Signal(1)
        last_good = Signal(1)
        index_pulse = Signal(1)
        count_direction = Signal(1)
        count_enable = Signal(1)
        ###
        self.comb += A.eq(pads.A)
        self.comb += B.eq(pads.B)
        self.comb += Z.eq(pads.Z)

        self.comb += self.out.eq(Cat(zl, i, c))
        self.sync += [
            Ad.eq(Cat(Ad[0:1], A)),
            Bd.eq(Cat(Bd[0:1], B))
        ]
        self.comb += [
            good_one.eq(Zc[2] & Zc[1] & Zc[0])
        ]
        #self.comb += [
        #    self._out.dat_w.eq(out),
        #    self._out.we.eq(True)
        #]


        self.comb += [
            good_zero.eq(~(Zc[2] | Zc[1] | Zc[0]))
        ]
        self.comb += [
            index_pulse.eq(good_one & ~last_good),
            count_enable.eq(Ad[1] ^ Ad[2] ^ Bd[1] ^ Bd[2]),
            count_direction.eq(Ad[1] ^ Bd[2])
        ]
        self.sync += [
            If(Z & ~good_one,
               Zc.eq(Zc + 1)
               ).Elif(~good_zero,
                      Zc.eq(Zc - 1)
                      ),
            If(good_one,
               last_good.eq(1)
               ).Elif(good_zero,
                      last_good.eq(0)
                      ),
            If(count_enable,
               If(count_direction,
                  c.eq(c + 1)
                  ).Else(
                   c.eq(c - 1)
               )
               ),
            If(index_pulse,
               i.eq(c),
               zl.eq(1)
               ).Elif(zr,
                      zl.eq(0)
                      )
        ]

#    This is a component of pluto_step, a hardware step waveform generator
#    Copyright 2007 Jeff Epler <jepler@unpythonic.net>
#
#    This program is free software; you can redistribute it and/or modify
#    it under the terms of the GNU General Public License as published by
#    the Free Software Foundation; either version 2 of the License, or
#    (at your option) any later version.
#
#    This program is distributed in the hope that it will be useful,
#    but WITHOUT ANY WARRANTY; without even the implied warranty of
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#    GNU General Public License for more details.
#
#    You should have received a copy of the GNU General Public License
#    along with this program; if not, write to the Free Software
#    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
class StepGen(Module):
    def __init__(self, pads):
        global W
        global F
        global T
        self.reset=ResetSignal()
        self.position=Signal(W+F)
        self.velocity = Signal(F+1)
        self.dirtime = Signal(T)
        self.steptime = Signal(T)
        self.enable = Signal(1)
        self.reset = Signal(1)
        self.tap = Signal(T)
        self.internal_dir = Signal(1)
        self.internal_step = Signal(1)
        self.inverse_dir = Signal(1)
        self.inverse_step = Signal(1)
        #self.debug = Signal(64)
        stepgen_local = Instance("stepgen",
            p_W = W,
            p_F = F,
            p_T = T,
            i_reset=self.reset,
            i_clk=ClockSignal(),
            i_enable=self.enable,
            o_out_position=self.position,
            i_velocity=self.velocity,
            i_dirtime=self.dirtime,
            i_steptime=self.steptime,
            i_tap=self.tap,
            o_step=self.internal_step,
            o_dir=self.internal_dir,
            #o_debug=self.debug
            )
        self.specials += stepgen_local
        self.comb += pads.dir.eq(self.internal_dir ^ self.inverse_dir)
        self.comb += pads.step.eq(self.internal_step ^ self.inverse_step)

class MMIO(Module,AutoCSR):
    def __init__(self):
        global W
        global F
        global T

        self.wallclock = Signal(64)

        self.velocity1 = CSRStorage(size=F+1, description="Stepgen velocity", write_from_dev=False, name='velocity1')
        self.velocity2 = CSRStorage(size=F+1, description="Stepgen velocity", write_from_dev=False, name='velocity2')
        self.velocity3 = CSRStorage(size=F+1, description="Stepgen velocity", write_from_dev=False, name='velocity3')
        self.velocity4 = CSRStorage(size=F+1, description="Stepgen velocity", write_from_dev=False, name='velocity4')
        self.velocity5 = CSRStorage(size=F+1, description="Stepgen velocity", write_from_dev=False, name='velocity5')
        self.velocity6 = CSRStorage(size=F+1, description="Stepgen velocity", write_from_dev=False, name='velocity6')

        self.dirtime = CSRStorage(size=T, description="Stepgen dirtime", write_from_dev=False, name='dirtime')
        self.steptime = CSRStorage(size=T, description="Stepgen steptime", write_from_dev=False, name='steptime')

        self.stepgens_ctrlword = CSRStorage(size=32, description="Stepgens control word", write_from_dev=False, name='stepgens_ctrlword')

        self.apply_time = CSRStorage(size=64, description="Apply time", write_from_dev=True, name='apply_time')
        self.gpios_out = CSRStorage(size=32, description="gpios_out", write_from_dev=False, name='gpios_out')
        i=0
        self.pwm_out0_width= CSRStorage(size=32, description="pwm{}_width".format(i), write_from_dev=False, name='pwm{}_width'.format(i))
        self.pwm_out0_period =CSRStorage(size=32, description="pwm{}_period".format(i), write_from_dev=False, name='pwm{}_period'.format(i))
        i = 1
        self.pwm_out1_width = CSRStorage(size=32, description="pwm{}_width".format(i), write_from_dev=False,
                                         name='pwm{}_width'.format(i))
        self.pwm_out1_period = CSRStorage(size=32, description="pwm{}_period".format(i), write_from_dev=False,
                                          name='pwm{}_period'.format(i))
        i = 2
        self.pwm_out2_width = CSRStorage(size=32, description="pwm{}_width".format(i), write_from_dev=False,
                                         name='pwm{}_width'.format(i))
        self.pwm_out2_period = CSRStorage(size=32, description="pwm{}_period".format(i), write_from_dev=False,
                                          name='pwm{}_period'.format(i))
        i = 3 
        self.pwm_out3_width= CSRStorage(size=32, description="pwm{}_width".format(i), write_from_dev=False, name='pwm{}_width'.format(i))
        self.pwm_out3_period =CSRStorage(size=32, description="pwm{}_period".format(i), write_from_dev=False, name='pwm{}_period'.format(i))
        i = 4
        self.pwm_out4_width = CSRStorage(size=32, description="pwm{}_width".format(i), write_from_dev=False,
                                         name='pwm{}_width'.format(i))
        self.pwm_out4_period = CSRStorage(size=32, description="pwm{}_period".format(i), write_from_dev=False,
                                          name='pwm{}_period'.format(i))
        i = 5
        self.pwm_out5_width = CSRStorage(size=32, description="pwm{}_width".format(i), write_from_dev=False,
                                         name='pwm{}_width'.format(i))
        self.pwm_out5_period = CSRStorage(size=32, description="pwm{}_period".format(i), write_from_dev=False,
                                          name='pwm{}_period'.format(i))


        self.position1 = CSRStatus(size=W + F, description="Stepgen 1 position", name='position1')
        self.position2 = CSRStatus(size=W + F, description="Stepgen 2 position", name='position2')
        self.position3 = CSRStatus(size=W + F, description="Stepgen 3 position", name='position3')
        self.position4 = CSRStatus(size=W + F, description="Stepgen 4 position", name='position4')
        self.position5 = CSRStatus(size=W + F, description="Stepgen 5 position", name='position5')
        self.position6 = CSRStatus(size=W + F, description="Stepgen 6 position", name='position6')
        self.wallclock = CSRStatus(size=64, description="wallclock time", name='wallclock')
        self.gpios_in = CSRStatus(size=32, description="gpios_in", name='gpios_in')
        self.encoder_position1 = CSRStatus(size=32, description="Encoder 1 position", name='encoder_position1')
        self.encoder_position2 = CSRStatus(size=32, description="Encoder 2 position", name='encoder_position2')
        self.encoder_position3 = CSRStatus(size=32, description="Encoder 3 position", name='encoder_position3')
        self.encoder_position4 = CSRStatus(size=32, description="Encoder 4 position", name='encoder_position4')
        self.encoder_position5 = CSRStatus(size=32, description="Encoder 5 position", name='encoder_position5')
        self.encoder_position6 = CSRStatus(size=32, description="Encoder 6 position", name='encoder_position6')

        #self.debug = CSRStatus(size=64, description="Stepgen1 debug", name='debug')

def _to_signal(obj):
    return obj.raw_bits() if isinstance(obj, Record) else obj

def add_pwm(self, default_width=512, default_period=1024, with_csr=True):
    from litex.soc.cores.pwm import PWM
    self.submodules.pwm = PWM(
        with_csr       = with_csr,
        default_enable = 0,
        default_width  = default_width,
        default_period = default_period
    )
    # Use PWM as Output Enable for pads.
    self.comb += If(~self.pwm.pwm, self.pads.eq(0))

class BenchSoC(SoCCore):
    def __init__(self, sys_clk_freq=int(50e6),mac_address=0x10e2d5000000, ip_address="192.168.0.50"):

        platform = colorlight_5a_75e.Platform()

        # SoCMini ----------------------------------------------------------------------------------
        SoCMini.__init__(self, platform, clk_freq=sys_clk_freq,
            ident          = "ColorLightCNC 5A-75E",
            ident_version  = True,
            #csr_address_width=16,
        )
        platform.add_extension(encoders)
        platform.add_extension(stepgens)
        # add verilog sources
        platform.add_source(os.path.join(os.getcwd(),"build",platform.name,"gateware","stepgen.v"))
        # CRG --------------------------------------------------------------------------------------
        self.submodules.crg = _CRG(platform, sys_clk_freq)

        # Etherbone --------------------------------------------------------------------------------
        self.submodules.ethphy = LiteEthPHYRGMII(
            clock_pads = self.platform.request("eth_clocks"),
            pads       = self.platform.request("eth"),
            tx_delay   = 0e-9,
            #rx_delay   = 3e-9,
            with_hw_init_reset = False)
        self.add_etherbone(phy=self.ethphy,
            buffer_depth=1060,
            mac_address=mac_address,
            ip_address=ip_address)


        #self.submodules.ethcore = LiteEthUDPIPCore(
        #    phy=self.ethphy,
        #    mac_address=mac_address,
        #    ip_address=ip_address,
        #    clk_freq=sys_clk_freq)

        #self.submodules.streamer = udp_streamer = LiteEthUDPStreamer(udp=self.ethcore.udp,
        #                                                             ip_address=convert_ip("192.168.0.50"),
        #                                                             udp_port=20000,
        #                                                          )
        #self.add
        #self.comb += udp_streamer.source.ready.eq(1)
        #my_led = platform.request("user_led_n")
        #self.sync += If(udp_streamer.source.valid, my_led.eq(udp_streamer.source.data[0]))

        #counter = Signal(26)
        #counter2 = Signal(32)

        #self.comb += udp_streamer.sink.last.eq(1)
        #self.sync += [
        #    counter.eq(counter + 1),
        #    If(counter < 4,
        #       udp_streamer.sink.data.eq(counter2),
        #       udp_streamer.sink.valid.eq(1),
        #       udp_streamer.sink.last.eq(0)
        #    ).Else(udp_streamer.sink.valid.eq(0),
        #       udp_streamer.sink.last.eq(1)
        #       ),
        #    If(udp_streamer.sink.valid & udp_streamer.sink.ready,
        #       udp_streamer.sink.valid.eq(0),
        #       counter2.eq(counter2 + 1)
        #    ),
        #]
        #for i in range (0,6):



        #udp_streamer.
        #broadcast_reply= Signal(1)

        #Record.connect()


        # SRAM -------------------------------------------------------------------------------------
        #self.add_ram("sram", 0x20000000, 0x1000)
        #encoders_data=Signal(256)

        #self.add_csr_region("sram",0x20000000,32,)

        self.submodules.encoder1 = encoder1 = QuadEnc(platform.request("encoderJ9_1", 0))
        self.submodules.encoder2 = encoder2 = QuadEnc(platform.request("encoderJ9_2", 1))
        self.submodules.encoder3 = encoder3 = QuadEnc(platform.request("encoderJ10_1", 2))
        self.submodules.encoder4 = encoder4 = QuadEnc(platform.request("encoderJ10_2", 3))
        self.submodules.encoder5 = encoder5 = QuadEnc(platform.request("encoderJ11_1", 4))
        self.submodules.encoder6 = encoder6 = QuadEnc(platform.request("encoderJ11_2", 5))

        self.submodules.stepgen1 = stepgen1 = StepGen(platform.request("stepgenJ1", 0))
        self.submodules.stepgen2 = stepgen2 = StepGen(platform.request("stepgenJ2", 1))
        self.submodules.stepgen3 = stepgen3 = StepGen(platform.request("stepgenJ3", 2))
        self.submodules.stepgen4 = stepgen4 = StepGen(platform.request("stepgenJ4", 3))
        self.submodules.stepgen5 = stepgen5 = StepGen(platform.request("stepgenJ5", 4))
        self.submodules.stepgen6 = stepgen6 = StepGen(platform.request("stepgenJ6", 5))
        self.submodules.MMIO_inst = MMIO_inst = MMIO()

        # GPIOs ------------------------------------------------------------------------------------
        platform.add_extension(_gpios_in)
        platform.add_extension(_gpios_out)
        platform.add_extension(_pwm_out)
        platform.add_extension(_en_out)
        
        #gpio_in = [Signal() for i in range(24)]

        self.gpio_in_pads  = platform.request_all("gpio_in")
        self.gpio_out_pads  = platform.request_all("gpio_out")
        self.pwm_out_pads  = platform.request_all("pwm_out")
        self.en_out_pad = platform.request("en_out")

        self.gpio_inputs = [(self.gpio_in_pads.l[i]) for i in range(0,30)]
        self.gpio_outputs = [(self.gpio_out_pads.l[i]) for i in range(0,32)]

        self.submodules.pwm0=pwm0=PWM(pwm=self.pwm_out_pads.l[0], default_enable=True, default_width=32, default_period=32, with_csr=False)
        self.submodules.pwm1=pwm1=PWM(pwm=self.pwm_out_pads.l[1], default_enable=True, default_width=32, default_period=32, with_csr=False)
        self.submodules.pwm2=pwm2=PWM(pwm=self.pwm_out_pads.l[2], default_enable=True, default_width=32, default_period=32, with_csr=False)
        self.submodules.pwm3=pwm3=PWM(pwm=self.pwm_out_pads.l[3], default_enable=True, default_width=32, default_period=32, with_csr=False)
        self.submodules.pwm4=pwm4=PWM(pwm=self.pwm_out_pads.l[4], default_enable=True, default_width=32, default_period=32, with_csr=False)
        self.submodules.pwm5=pwm5=PWM(pwm=self.pwm_out_pads.l[5], default_enable=True, default_width=32, default_period=32, with_csr=False)

        self.sync += \
            [
        self.MMIO_inst.gpios_in.status.eq(Cat([self.gpio_inputs[i] for i in range(0,30)])),
        self.MMIO_inst.gpios_in.we.eq(True)
            ]

        self.sync += [
            self.MMIO_inst.position1.status.eq(self.stepgen1.position),
            self.MMIO_inst.position1.we.eq(True),
            self.MMIO_inst.position2.status.eq(self.stepgen2.position),
            self.MMIO_inst.position2.we.eq(True),
            self.MMIO_inst.position3.status.eq(self.stepgen3.position),
            self.MMIO_inst.position3.we.eq(True),
            self.MMIO_inst.position4.status.eq(self.stepgen4.position),
            self.MMIO_inst.position4.we.eq(True),
            self.MMIO_inst.position5.status.eq(self.stepgen5.position),
            self.MMIO_inst.position5.we.eq(True),
            self.MMIO_inst.position6.status.eq(self.stepgen6.position),
            self.MMIO_inst.position6.we.eq(True),

            self.MMIO_inst.encoder_position1.status.eq(self.encoder1.out),
            self.MMIO_inst.encoder_position1.we.eq(True),
            self.MMIO_inst.encoder_position2.status.eq(self.encoder2.out),
            self.MMIO_inst.encoder_position2.we.eq(True),
            self.MMIO_inst.encoder_position3.status.eq(self.encoder3.out),
            self.MMIO_inst.encoder_position3.we.eq(True),
            self.MMIO_inst.encoder_position4.status.eq(self.encoder4.out),
            self.MMIO_inst.encoder_position4.we.eq(True),
            self.MMIO_inst.encoder_position5.status.eq(self.encoder5.out),
            self.MMIO_inst.encoder_position5.we.eq(True),
            self.MMIO_inst.encoder_position6.status.eq(self.encoder6.out),
            self.MMIO_inst.encoder_position6.we.eq(True),

            self.stepgen1.enable.eq(self.MMIO_inst.stepgens_ctrlword.storage[0]),
            self.stepgen1.reset.eq(self.MMIO_inst.stepgens_ctrlword.storage[1]),
            self.stepgen1.inverse_step.eq(self.MMIO_inst.stepgens_ctrlword.storage[10]),
            self.stepgen1.inverse_dir.eq(self.MMIO_inst.stepgens_ctrlword.storage[11]),

            self.stepgen2.enable.eq(self.MMIO_inst.stepgens_ctrlword.storage[0]),
            self.stepgen2.reset.eq(self.MMIO_inst.stepgens_ctrlword.storage[1]),
            self.stepgen2.inverse_step.eq(self.MMIO_inst.stepgens_ctrlword.storage[12]),
            self.stepgen2.inverse_dir.eq(self.MMIO_inst.stepgens_ctrlword.storage[13]),

            self.stepgen3.enable.eq(self.MMIO_inst.stepgens_ctrlword.storage[0]),
            self.stepgen3.reset.eq(self.MMIO_inst.stepgens_ctrlword.storage[1]),
            self.stepgen3.inverse_step.eq(self.MMIO_inst.stepgens_ctrlword.storage[14]),
            self.stepgen3.inverse_dir.eq(self.MMIO_inst.stepgens_ctrlword.storage[15]),

            self.stepgen4.enable.eq(self.MMIO_inst.stepgens_ctrlword.storage[0]),
            self.stepgen4.reset.eq(self.MMIO_inst.stepgens_ctrlword.storage[1]),
            self.stepgen4.inverse_step.eq(self.MMIO_inst.stepgens_ctrlword.storage[16]),
            self.stepgen4.inverse_dir.eq(self.MMIO_inst.stepgens_ctrlword.storage[17]),

            self.stepgen5.enable.eq(self.MMIO_inst.stepgens_ctrlword.storage[0]),
            self.stepgen5.reset.eq(self.MMIO_inst.stepgens_ctrlword.storage[1]),
            self.stepgen5.inverse_step.eq(self.MMIO_inst.stepgens_ctrlword.storage[18]),
            self.stepgen5.inverse_dir.eq(self.MMIO_inst.stepgens_ctrlword.storage[19]),

            self.stepgen6.enable.eq(self.MMIO_inst.stepgens_ctrlword.storage[0]),
            self.stepgen6.reset.eq(self.MMIO_inst.stepgens_ctrlword.storage[1]),
            self.stepgen6.inverse_step.eq(self.MMIO_inst.stepgens_ctrlword.storage[20]),
            self.stepgen6.inverse_dir.eq(self.MMIO_inst.stepgens_ctrlword.storage[21]),

            self.stepgen1.dirtime.eq(self.MMIO_inst.dirtime.storage),
            self.stepgen1.steptime.eq(self.MMIO_inst.steptime.storage),
            self.stepgen2.dirtime.eq(self.MMIO_inst.dirtime.storage),
            self.stepgen2.steptime.eq(self.MMIO_inst.steptime.storage),
            self.stepgen3.dirtime.eq(self.MMIO_inst.dirtime.storage),
            self.stepgen3.steptime.eq(self.MMIO_inst.steptime.storage),
            self.stepgen4.dirtime.eq(self.MMIO_inst.dirtime.storage),
            self.stepgen4.steptime.eq(self.MMIO_inst.steptime.storage),
            self.stepgen5.dirtime.eq(self.MMIO_inst.dirtime.storage),
            self.stepgen5.steptime.eq(self.MMIO_inst.steptime.storage),
            self.stepgen6.dirtime.eq(self.MMIO_inst.dirtime.storage),
            self.stepgen6.steptime.eq(self.MMIO_inst.steptime.storage),

            self.MMIO_inst.wallclock.status.eq(self.MMIO_inst.wallclock.status+1),

            self.en_out_pad.eq(self.MMIO_inst.stepgens_ctrlword.storage[0]),

            If(self.MMIO_inst.stepgens_ctrlword.storage[2],
            If(self.MMIO_inst.wallclock.status >= self.MMIO_inst.apply_time.storage,
            self.MMIO_inst.apply_time.dat_w.eq(0), #apply_time - timestamp of wall clock when to apply stepgen settings
            self.MMIO_inst.apply_time.we.eq(True),
            self.stepgen1.velocity.eq(self.MMIO_inst.velocity1.storage),
            self.stepgen2.velocity.eq(self.MMIO_inst.velocity2.storage),
            self.stepgen3.velocity.eq(self.MMIO_inst.velocity3.storage),
            self.stepgen4.velocity.eq(self.MMIO_inst.velocity4.storage),
            self.stepgen5.velocity.eq(self.MMIO_inst.velocity5.storage),
            self.stepgen6.velocity.eq(self.MMIO_inst.velocity6.storage),
            ).Else(
                #self.MMIO_inst.apply_time.dat_w.eq(0),
                self.MMIO_inst.apply_time.we.eq(False),
            )).Else(
                self.stepgen1.velocity.eq(self.MMIO_inst.velocity1.storage),
                self.stepgen2.velocity.eq(self.MMIO_inst.velocity2.storage),
                self.stepgen3.velocity.eq(self.MMIO_inst.velocity3.storage),
                self.stepgen4.velocity.eq(self.MMIO_inst.velocity4.storage),
                self.stepgen5.velocity.eq(self.MMIO_inst.velocity5.storage),
                self.stepgen6.velocity.eq(self.MMIO_inst.velocity6.storage),
            )

        ]

        self.sync+=[self.gpio_outputs[i].eq(self.MMIO_inst.gpios_out.storage[i]) for i in range(0,32)]
        self.sync+=[self.pwm0.width.eq(self.MMIO_inst.pwm_out0_width.storage),
                    self.pwm0.period.eq(self.MMIO_inst.pwm_out0_period.storage),
                    self.pwm1.width.eq(self.MMIO_inst.pwm_out1_width.storage),
                    self.pwm1.period.eq(self.MMIO_inst.pwm_out1_period.storage),
                    self.pwm2.width.eq(self.MMIO_inst.pwm_out2_width.storage),
                    self.pwm2.period.eq(self.MMIO_inst.pwm_out2_period.storage),
					self.pwm3.width.eq(self.MMIO_inst.pwm_out3_width.storage),
                    self.pwm3.period.eq(self.MMIO_inst.pwm_out3_period.storage),
                    self.pwm4.width.eq(self.MMIO_inst.pwm_out4_width.storage),
                    self.pwm4.period.eq(self.MMIO_inst.pwm_out4_period.storage),
                    self.pwm5.width.eq(self.MMIO_inst.pwm_out5_width.storage),
                    self.pwm5.period.eq(self.MMIO_inst.pwm_out5_period.storage)                    
                    ]


        #for i in range(24,31):
        #    self.add_csr("gpio{}".format(i))


        # Leds -------------------------------------------------------------------------------------
        #from litex.soc.cores.led import LedChaser
        self.submodules.leds = LedChaser(
            pads         = platform.request_all("user_led_n"),
            sys_clk_freq = sys_clk_freq)
        self.leds.add_pwm()


        #def __init__(self, ip, ip_address, debug=False, loopback=False):

        #quadenc = []
        #for i in range(0, 6):
        #    quadenc.append(QuadEnc(platform.request("encoderJ" + str(i+1), i)))
        #    self.submodules.encoder[i] = quadenc[i]

# Main ---------------------------------------------------------------------------------------------

def main():
    parser = argparse.ArgumentParser(description="LiteEth Bench on ColorLight 5A-75E")
    parser.add_argument("--build", action="store_true", help="Build bitstream")
    parser.add_argument("--load",  action="store_true", help="Load bitstream")
    args = parser.parse_args()

    soc     = BenchSoC()
    builder = Builder(soc, csr_csv="csr.csv")
    builder.build(run=args.build)

    if args.load:
        prog = soc.platform.create_programmer()
        prog.load_bitstream(os.path.join(builder.gateware_dir, soc.build_name + ".svf"))

if __name__ == "__main__":
    main()
