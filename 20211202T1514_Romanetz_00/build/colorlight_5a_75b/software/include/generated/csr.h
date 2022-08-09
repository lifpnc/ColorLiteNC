//--------------------------------------------------------------------------------
// Auto-generated by Migen (bee558c) & LiteX (756503ab) on 2021-12-01 02:31:36
//--------------------------------------------------------------------------------
#include <generated/soc.h>
#ifndef __GENERATED_CSR_H
#define __GENERATED_CSR_H
#include <stdint.h>
#include <system.h>
#ifndef CSR_ACCESSORS_DEFINED
#include <hw/common.h>
#endif /* ! CSR_ACCESSORS_DEFINED */
#ifndef CSR_BASE
#define CSR_BASE 0x0L
#endif

/* MMIO_inst */
#define CSR_MMIO_INST_BASE (CSR_BASE + 0x0L)
#define CSR_MMIO_INST_VELOCITY1_ADDR (CSR_BASE + 0x0L)
#define CSR_MMIO_INST_VELOCITY1_SIZE 1
static inline uint32_t MMIO_inst_velocity1_read(void) {
	return csr_read_simple(CSR_BASE + 0x0L);
}
static inline void MMIO_inst_velocity1_write(uint32_t v) {
	csr_write_simple(v, CSR_BASE + 0x0L);
}
#define CSR_MMIO_INST_VELOCITY2_ADDR (CSR_BASE + 0x4L)
#define CSR_MMIO_INST_VELOCITY2_SIZE 1
static inline uint32_t MMIO_inst_velocity2_read(void) {
	return csr_read_simple(CSR_BASE + 0x4L);
}
static inline void MMIO_inst_velocity2_write(uint32_t v) {
	csr_write_simple(v, CSR_BASE + 0x4L);
}
#define CSR_MMIO_INST_VELOCITY3_ADDR (CSR_BASE + 0x8L)
#define CSR_MMIO_INST_VELOCITY3_SIZE 1
static inline uint32_t MMIO_inst_velocity3_read(void) {
	return csr_read_simple(CSR_BASE + 0x8L);
}
static inline void MMIO_inst_velocity3_write(uint32_t v) {
	csr_write_simple(v, CSR_BASE + 0x8L);
}
#define CSR_MMIO_INST_VELOCITY4_ADDR (CSR_BASE + 0xcL)
#define CSR_MMIO_INST_VELOCITY4_SIZE 1
static inline uint32_t MMIO_inst_velocity4_read(void) {
	return csr_read_simple(CSR_BASE + 0xcL);
}
static inline void MMIO_inst_velocity4_write(uint32_t v) {
	csr_write_simple(v, CSR_BASE + 0xcL);
}
#define CSR_MMIO_INST_VELOCITY5_ADDR (CSR_BASE + 0x10L)
#define CSR_MMIO_INST_VELOCITY5_SIZE 1
static inline uint32_t MMIO_inst_velocity5_read(void) {
	return csr_read_simple(CSR_BASE + 0x10L);
}
static inline void MMIO_inst_velocity5_write(uint32_t v) {
	csr_write_simple(v, CSR_BASE + 0x10L);
}
#define CSR_MMIO_INST_VELOCITY6_ADDR (CSR_BASE + 0x14L)
#define CSR_MMIO_INST_VELOCITY6_SIZE 1
static inline uint32_t MMIO_inst_velocity6_read(void) {
	return csr_read_simple(CSR_BASE + 0x14L);
}
static inline void MMIO_inst_velocity6_write(uint32_t v) {
	csr_write_simple(v, CSR_BASE + 0x14L);
}
#define CSR_MMIO_INST_DIRTIME_ADDR (CSR_BASE + 0x18L)
#define CSR_MMIO_INST_DIRTIME_SIZE 1
static inline uint32_t MMIO_inst_dirtime_read(void) {
	return csr_read_simple(CSR_BASE + 0x18L);
}
static inline void MMIO_inst_dirtime_write(uint32_t v) {
	csr_write_simple(v, CSR_BASE + 0x18L);
}
#define CSR_MMIO_INST_STEPTIME_ADDR (CSR_BASE + 0x1cL)
#define CSR_MMIO_INST_STEPTIME_SIZE 1
static inline uint32_t MMIO_inst_steptime_read(void) {
	return csr_read_simple(CSR_BASE + 0x1cL);
}
static inline void MMIO_inst_steptime_write(uint32_t v) {
	csr_write_simple(v, CSR_BASE + 0x1cL);
}
#define CSR_MMIO_INST_STEPGENS_CTRLWORD_ADDR (CSR_BASE + 0x20L)
#define CSR_MMIO_INST_STEPGENS_CTRLWORD_SIZE 1
static inline uint32_t MMIO_inst_stepgens_ctrlword_read(void) {
	return csr_read_simple(CSR_BASE + 0x20L);
}
static inline void MMIO_inst_stepgens_ctrlword_write(uint32_t v) {
	csr_write_simple(v, CSR_BASE + 0x20L);
}
#define CSR_MMIO_INST_APPLY_TIME_ADDR (CSR_BASE + 0x24L)
#define CSR_MMIO_INST_APPLY_TIME_SIZE 2
static inline uint64_t MMIO_inst_apply_time_read(void) {
	uint64_t r = csr_read_simple(CSR_BASE + 0x24L);
	r <<= 32;
	r |= csr_read_simple(CSR_BASE + 0x28L);
	return r;
}
static inline void MMIO_inst_apply_time_write(uint64_t v) {
	csr_write_simple(v >> 32, CSR_BASE + 0x24L);
	csr_write_simple(v, CSR_BASE + 0x28L);
}
#define CSR_MMIO_INST_GPIOS_OUT_ADDR (CSR_BASE + 0x2cL)
#define CSR_MMIO_INST_GPIOS_OUT_SIZE 1
static inline uint32_t MMIO_inst_gpios_out_read(void) {
	return csr_read_simple(CSR_BASE + 0x2cL);
}
static inline void MMIO_inst_gpios_out_write(uint32_t v) {
	csr_write_simple(v, CSR_BASE + 0x2cL);
}
#define CSR_MMIO_INST_PWM0_WIDTH_ADDR (CSR_BASE + 0x30L)
#define CSR_MMIO_INST_PWM0_WIDTH_SIZE 1
static inline uint32_t MMIO_inst_pwm0_width_read(void) {
	return csr_read_simple(CSR_BASE + 0x30L);
}
static inline void MMIO_inst_pwm0_width_write(uint32_t v) {
	csr_write_simple(v, CSR_BASE + 0x30L);
}
#define CSR_MMIO_INST_PWM0_PERIOD_ADDR (CSR_BASE + 0x34L)
#define CSR_MMIO_INST_PWM0_PERIOD_SIZE 1
static inline uint32_t MMIO_inst_pwm0_period_read(void) {
	return csr_read_simple(CSR_BASE + 0x34L);
}
static inline void MMIO_inst_pwm0_period_write(uint32_t v) {
	csr_write_simple(v, CSR_BASE + 0x34L);
}
#define CSR_MMIO_INST_PWM1_WIDTH_ADDR (CSR_BASE + 0x38L)
#define CSR_MMIO_INST_PWM1_WIDTH_SIZE 1
static inline uint32_t MMIO_inst_pwm1_width_read(void) {
	return csr_read_simple(CSR_BASE + 0x38L);
}
static inline void MMIO_inst_pwm1_width_write(uint32_t v) {
	csr_write_simple(v, CSR_BASE + 0x38L);
}
#define CSR_MMIO_INST_PWM1_PERIOD_ADDR (CSR_BASE + 0x3cL)
#define CSR_MMIO_INST_PWM1_PERIOD_SIZE 1
static inline uint32_t MMIO_inst_pwm1_period_read(void) {
	return csr_read_simple(CSR_BASE + 0x3cL);
}
static inline void MMIO_inst_pwm1_period_write(uint32_t v) {
	csr_write_simple(v, CSR_BASE + 0x3cL);
}
#define CSR_MMIO_INST_PWM2_WIDTH_ADDR (CSR_BASE + 0x40L)
#define CSR_MMIO_INST_PWM2_WIDTH_SIZE 1
static inline uint32_t MMIO_inst_pwm2_width_read(void) {
	return csr_read_simple(CSR_BASE + 0x40L);
}
static inline void MMIO_inst_pwm2_width_write(uint32_t v) {
	csr_write_simple(v, CSR_BASE + 0x40L);
}
#define CSR_MMIO_INST_PWM2_PERIOD_ADDR (CSR_BASE + 0x44L)
#define CSR_MMIO_INST_PWM2_PERIOD_SIZE 1
static inline uint32_t MMIO_inst_pwm2_period_read(void) {
	return csr_read_simple(CSR_BASE + 0x44L);
}
static inline void MMIO_inst_pwm2_period_write(uint32_t v) {
	csr_write_simple(v, CSR_BASE + 0x44L);
}
#define CSR_MMIO_INST_POSITION1_ADDR (CSR_BASE + 0x48L)
#define CSR_MMIO_INST_POSITION1_SIZE 2
static inline uint64_t MMIO_inst_position1_read(void) {
	uint64_t r = csr_read_simple(CSR_BASE + 0x48L);
	r <<= 32;
	r |= csr_read_simple(CSR_BASE + 0x4cL);
	return r;
}
#define CSR_MMIO_INST_POSITION2_ADDR (CSR_BASE + 0x50L)
#define CSR_MMIO_INST_POSITION2_SIZE 2
static inline uint64_t MMIO_inst_position2_read(void) {
	uint64_t r = csr_read_simple(CSR_BASE + 0x50L);
	r <<= 32;
	r |= csr_read_simple(CSR_BASE + 0x54L);
	return r;
}
#define CSR_MMIO_INST_POSITION3_ADDR (CSR_BASE + 0x58L)
#define CSR_MMIO_INST_POSITION3_SIZE 2
static inline uint64_t MMIO_inst_position3_read(void) {
	uint64_t r = csr_read_simple(CSR_BASE + 0x58L);
	r <<= 32;
	r |= csr_read_simple(CSR_BASE + 0x5cL);
	return r;
}
#define CSR_MMIO_INST_POSITION4_ADDR (CSR_BASE + 0x60L)
#define CSR_MMIO_INST_POSITION4_SIZE 2
static inline uint64_t MMIO_inst_position4_read(void) {
	uint64_t r = csr_read_simple(CSR_BASE + 0x60L);
	r <<= 32;
	r |= csr_read_simple(CSR_BASE + 0x64L);
	return r;
}
#define CSR_MMIO_INST_POSITION5_ADDR (CSR_BASE + 0x68L)
#define CSR_MMIO_INST_POSITION5_SIZE 2
static inline uint64_t MMIO_inst_position5_read(void) {
	uint64_t r = csr_read_simple(CSR_BASE + 0x68L);
	r <<= 32;
	r |= csr_read_simple(CSR_BASE + 0x6cL);
	return r;
}
#define CSR_MMIO_INST_POSITION6_ADDR (CSR_BASE + 0x70L)
#define CSR_MMIO_INST_POSITION6_SIZE 2
static inline uint64_t MMIO_inst_position6_read(void) {
	uint64_t r = csr_read_simple(CSR_BASE + 0x70L);
	r <<= 32;
	r |= csr_read_simple(CSR_BASE + 0x74L);
	return r;
}
#define CSR_MMIO_INST_WALLCLOCK_ADDR (CSR_BASE + 0x78L)
#define CSR_MMIO_INST_WALLCLOCK_SIZE 2
static inline uint64_t MMIO_inst_wallclock_read(void) {
	uint64_t r = csr_read_simple(CSR_BASE + 0x78L);
	r <<= 32;
	r |= csr_read_simple(CSR_BASE + 0x7cL);
	return r;
}
#define CSR_MMIO_INST_GPIOS_IN_ADDR (CSR_BASE + 0x80L)
#define CSR_MMIO_INST_GPIOS_IN_SIZE 1
static inline uint32_t MMIO_inst_gpios_in_read(void) {
	return csr_read_simple(CSR_BASE + 0x80L);
}

/* ctrl */
#define CSR_CTRL_BASE (CSR_BASE + 0x800L)
#define CSR_CTRL_RESET_ADDR (CSR_BASE + 0x800L)
#define CSR_CTRL_RESET_SIZE 1
static inline uint32_t ctrl_reset_read(void) {
	return csr_read_simple(CSR_BASE + 0x800L);
}
static inline void ctrl_reset_write(uint32_t v) {
	csr_write_simple(v, CSR_BASE + 0x800L);
}
#define CSR_CTRL_RESET_SOC_RST_OFFSET 0
#define CSR_CTRL_RESET_SOC_RST_SIZE 1
static inline uint32_t ctrl_reset_soc_rst_extract(uint32_t oldword) {
	uint32_t mask = ((1 << 1)-1);
	return ( (oldword >> 0) & mask );
}
static inline uint32_t ctrl_reset_soc_rst_read(void) {
	uint32_t word = ctrl_reset_read();
	return ctrl_reset_soc_rst_extract(word);
}
static inline uint32_t ctrl_reset_soc_rst_replace(uint32_t oldword, uint32_t plain_value) {
	uint32_t mask = ((1 << 1)-1);
	return (oldword & (~(mask << 0))) | (mask & plain_value)<< 0 ;
}
static inline void ctrl_reset_soc_rst_write(uint32_t plain_value) {
	uint32_t oldword = ctrl_reset_read();
	uint32_t newword = ctrl_reset_soc_rst_replace(oldword, plain_value);
	ctrl_reset_write(newword);
}
#define CSR_CTRL_RESET_CPU_RST_OFFSET 1
#define CSR_CTRL_RESET_CPU_RST_SIZE 1
static inline uint32_t ctrl_reset_cpu_rst_extract(uint32_t oldword) {
	uint32_t mask = ((1 << 1)-1);
	return ( (oldword >> 1) & mask );
}
static inline uint32_t ctrl_reset_cpu_rst_read(void) {
	uint32_t word = ctrl_reset_read();
	return ctrl_reset_cpu_rst_extract(word);
}
static inline uint32_t ctrl_reset_cpu_rst_replace(uint32_t oldword, uint32_t plain_value) {
	uint32_t mask = ((1 << 1)-1);
	return (oldword & (~(mask << 1))) | (mask & plain_value)<< 1 ;
}
static inline void ctrl_reset_cpu_rst_write(uint32_t plain_value) {
	uint32_t oldword = ctrl_reset_read();
	uint32_t newword = ctrl_reset_cpu_rst_replace(oldword, plain_value);
	ctrl_reset_write(newword);
}
#define CSR_CTRL_SCRATCH_ADDR (CSR_BASE + 0x804L)
#define CSR_CTRL_SCRATCH_SIZE 1
static inline uint32_t ctrl_scratch_read(void) {
	return csr_read_simple(CSR_BASE + 0x804L);
}
static inline void ctrl_scratch_write(uint32_t v) {
	csr_write_simple(v, CSR_BASE + 0x804L);
}
#define CSR_CTRL_BUS_ERRORS_ADDR (CSR_BASE + 0x808L)
#define CSR_CTRL_BUS_ERRORS_SIZE 1
static inline uint32_t ctrl_bus_errors_read(void) {
	return csr_read_simple(CSR_BASE + 0x808L);
}

/* ethphy */
#define CSR_ETHPHY_BASE (CSR_BASE + 0x1000L)
#define CSR_ETHPHY_CRG_RESET_ADDR (CSR_BASE + 0x1000L)
#define CSR_ETHPHY_CRG_RESET_SIZE 1
static inline uint32_t ethphy_crg_reset_read(void) {
	return csr_read_simple(CSR_BASE + 0x1000L);
}
static inline void ethphy_crg_reset_write(uint32_t v) {
	csr_write_simple(v, CSR_BASE + 0x1000L);
}
#define CSR_ETHPHY_RX_INBAND_STATUS_ADDR (CSR_BASE + 0x1004L)
#define CSR_ETHPHY_RX_INBAND_STATUS_SIZE 1
static inline uint32_t ethphy_rx_inband_status_read(void) {
	return csr_read_simple(CSR_BASE + 0x1004L);
}
#define CSR_ETHPHY_RX_INBAND_STATUS_LINK_STATUS_OFFSET 0
#define CSR_ETHPHY_RX_INBAND_STATUS_LINK_STATUS_SIZE 1
static inline uint32_t ethphy_rx_inband_status_link_status_extract(uint32_t oldword) {
	uint32_t mask = ((1 << 1)-1);
	return ( (oldword >> 0) & mask );
}
static inline uint32_t ethphy_rx_inband_status_link_status_read(void) {
	uint32_t word = ethphy_rx_inband_status_read();
	return ethphy_rx_inband_status_link_status_extract(word);
}
#define CSR_ETHPHY_RX_INBAND_STATUS_CLOCK_SPEED_OFFSET 1
#define CSR_ETHPHY_RX_INBAND_STATUS_CLOCK_SPEED_SIZE 1
static inline uint32_t ethphy_rx_inband_status_clock_speed_extract(uint32_t oldword) {
	uint32_t mask = ((1 << 1)-1);
	return ( (oldword >> 1) & mask );
}
static inline uint32_t ethphy_rx_inband_status_clock_speed_read(void) {
	uint32_t word = ethphy_rx_inband_status_read();
	return ethphy_rx_inband_status_clock_speed_extract(word);
}
#define CSR_ETHPHY_RX_INBAND_STATUS_DUPLEX_STATUS_OFFSET 2
#define CSR_ETHPHY_RX_INBAND_STATUS_DUPLEX_STATUS_SIZE 1
static inline uint32_t ethphy_rx_inband_status_duplex_status_extract(uint32_t oldword) {
	uint32_t mask = ((1 << 1)-1);
	return ( (oldword >> 2) & mask );
}
static inline uint32_t ethphy_rx_inband_status_duplex_status_read(void) {
	uint32_t word = ethphy_rx_inband_status_read();
	return ethphy_rx_inband_status_duplex_status_extract(word);
}
#define CSR_ETHPHY_MDIO_W_ADDR (CSR_BASE + 0x1008L)
#define CSR_ETHPHY_MDIO_W_SIZE 1
static inline uint32_t ethphy_mdio_w_read(void) {
	return csr_read_simple(CSR_BASE + 0x1008L);
}
static inline void ethphy_mdio_w_write(uint32_t v) {
	csr_write_simple(v, CSR_BASE + 0x1008L);
}
#define CSR_ETHPHY_MDIO_W_MDC_OFFSET 0
#define CSR_ETHPHY_MDIO_W_MDC_SIZE 1
static inline uint32_t ethphy_mdio_w_mdc_extract(uint32_t oldword) {
	uint32_t mask = ((1 << 1)-1);
	return ( (oldword >> 0) & mask );
}
static inline uint32_t ethphy_mdio_w_mdc_read(void) {
	uint32_t word = ethphy_mdio_w_read();
	return ethphy_mdio_w_mdc_extract(word);
}
static inline uint32_t ethphy_mdio_w_mdc_replace(uint32_t oldword, uint32_t plain_value) {
	uint32_t mask = ((1 << 1)-1);
	return (oldword & (~(mask << 0))) | (mask & plain_value)<< 0 ;
}
static inline void ethphy_mdio_w_mdc_write(uint32_t plain_value) {
	uint32_t oldword = ethphy_mdio_w_read();
	uint32_t newword = ethphy_mdio_w_mdc_replace(oldword, plain_value);
	ethphy_mdio_w_write(newword);
}
#define CSR_ETHPHY_MDIO_W_OE_OFFSET 1
#define CSR_ETHPHY_MDIO_W_OE_SIZE 1
static inline uint32_t ethphy_mdio_w_oe_extract(uint32_t oldword) {
	uint32_t mask = ((1 << 1)-1);
	return ( (oldword >> 1) & mask );
}
static inline uint32_t ethphy_mdio_w_oe_read(void) {
	uint32_t word = ethphy_mdio_w_read();
	return ethphy_mdio_w_oe_extract(word);
}
static inline uint32_t ethphy_mdio_w_oe_replace(uint32_t oldword, uint32_t plain_value) {
	uint32_t mask = ((1 << 1)-1);
	return (oldword & (~(mask << 1))) | (mask & plain_value)<< 1 ;
}
static inline void ethphy_mdio_w_oe_write(uint32_t plain_value) {
	uint32_t oldword = ethphy_mdio_w_read();
	uint32_t newword = ethphy_mdio_w_oe_replace(oldword, plain_value);
	ethphy_mdio_w_write(newword);
}
#define CSR_ETHPHY_MDIO_W_W_OFFSET 2
#define CSR_ETHPHY_MDIO_W_W_SIZE 1
static inline uint32_t ethphy_mdio_w_w_extract(uint32_t oldword) {
	uint32_t mask = ((1 << 1)-1);
	return ( (oldword >> 2) & mask );
}
static inline uint32_t ethphy_mdio_w_w_read(void) {
	uint32_t word = ethphy_mdio_w_read();
	return ethphy_mdio_w_w_extract(word);
}
static inline uint32_t ethphy_mdio_w_w_replace(uint32_t oldword, uint32_t plain_value) {
	uint32_t mask = ((1 << 1)-1);
	return (oldword & (~(mask << 2))) | (mask & plain_value)<< 2 ;
}
static inline void ethphy_mdio_w_w_write(uint32_t plain_value) {
	uint32_t oldword = ethphy_mdio_w_read();
	uint32_t newword = ethphy_mdio_w_w_replace(oldword, plain_value);
	ethphy_mdio_w_write(newword);
}
#define CSR_ETHPHY_MDIO_R_ADDR (CSR_BASE + 0x100cL)
#define CSR_ETHPHY_MDIO_R_SIZE 1
static inline uint32_t ethphy_mdio_r_read(void) {
	return csr_read_simple(CSR_BASE + 0x100cL);
}
#define CSR_ETHPHY_MDIO_R_R_OFFSET 0
#define CSR_ETHPHY_MDIO_R_R_SIZE 1
static inline uint32_t ethphy_mdio_r_r_extract(uint32_t oldword) {
	uint32_t mask = ((1 << 1)-1);
	return ( (oldword >> 0) & mask );
}
static inline uint32_t ethphy_mdio_r_r_read(void) {
	uint32_t word = ethphy_mdio_r_read();
	return ethphy_mdio_r_r_extract(word);
}

/* identifier_mem */
#define CSR_IDENTIFIER_MEM_BASE (CSR_BASE + 0x1800L)

/* leds */
#define CSR_LEDS_BASE (CSR_BASE + 0x2000L)
#define CSR_LEDS_OUT_ADDR (CSR_BASE + 0x2000L)
#define CSR_LEDS_OUT_SIZE 1
static inline uint32_t leds_out_read(void) {
	return csr_read_simple(CSR_BASE + 0x2000L);
}
static inline void leds_out_write(uint32_t v) {
	csr_write_simple(v, CSR_BASE + 0x2000L);
}
#define CSR_LEDS_PWM_ENABLE_ADDR (CSR_BASE + 0x2004L)
#define CSR_LEDS_PWM_ENABLE_SIZE 1
static inline uint32_t leds_pwm_enable_read(void) {
	return csr_read_simple(CSR_BASE + 0x2004L);
}
static inline void leds_pwm_enable_write(uint32_t v) {
	csr_write_simple(v, CSR_BASE + 0x2004L);
}
#define CSR_LEDS_PWM_WIDTH_ADDR (CSR_BASE + 0x2008L)
#define CSR_LEDS_PWM_WIDTH_SIZE 1
static inline uint32_t leds_pwm_width_read(void) {
	return csr_read_simple(CSR_BASE + 0x2008L);
}
static inline void leds_pwm_width_write(uint32_t v) {
	csr_write_simple(v, CSR_BASE + 0x2008L);
}
#define CSR_LEDS_PWM_PERIOD_ADDR (CSR_BASE + 0x200cL)
#define CSR_LEDS_PWM_PERIOD_SIZE 1
static inline uint32_t leds_pwm_period_read(void) {
	return csr_read_simple(CSR_BASE + 0x200cL);
}
static inline void leds_pwm_period_write(uint32_t v) {
	csr_write_simple(v, CSR_BASE + 0x200cL);
}

#endif