//--------------------------------------------------------------------------------
// Auto-generated by Migen (ac70301) & LiteX (7712c8a7) on 2022-04-13 18:49:19
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
#define CSR_MMIO_INST_WATCHDOG_DATA_ADDR (CSR_BASE + 0x0L)
#define CSR_MMIO_INST_WATCHDOG_DATA_SIZE 1
static inline uint32_t MMIO_inst_watchdog_data_read(void) {
	return csr_read_simple(CSR_BASE + 0x0L);
}
static inline void MMIO_inst_watchdog_data_write(uint32_t v) {
	csr_write_simple(v, CSR_BASE + 0x0L);
}
#define CSR_MMIO_INST_GPIO_OUT_ADDR (CSR_BASE + 0x4L)
#define CSR_MMIO_INST_GPIO_OUT_SIZE 1
static inline uint32_t MMIO_inst_gpio_out_read(void) {
	return csr_read_simple(CSR_BASE + 0x4L);
}
static inline void MMIO_inst_gpio_out_write(uint32_t v) {
	csr_write_simple(v, CSR_BASE + 0x4L);
}
#define CSR_MMIO_INST_PWM_0_ENABLE_ADDR (CSR_BASE + 0x8L)
#define CSR_MMIO_INST_PWM_0_ENABLE_SIZE 1
static inline uint32_t MMIO_inst_pwm_0_enable_read(void) {
	return csr_read_simple(CSR_BASE + 0x8L);
}
static inline void MMIO_inst_pwm_0_enable_write(uint32_t v) {
	csr_write_simple(v, CSR_BASE + 0x8L);
}
#define CSR_MMIO_INST_PWM_0_PERIOD_ADDR (CSR_BASE + 0xcL)
#define CSR_MMIO_INST_PWM_0_PERIOD_SIZE 1
static inline uint32_t MMIO_inst_pwm_0_period_read(void) {
	return csr_read_simple(CSR_BASE + 0xcL);
}
static inline void MMIO_inst_pwm_0_period_write(uint32_t v) {
	csr_write_simple(v, CSR_BASE + 0xcL);
}
#define CSR_MMIO_INST_PWM_0_WIDTH_ADDR (CSR_BASE + 0x10L)
#define CSR_MMIO_INST_PWM_0_WIDTH_SIZE 1
static inline uint32_t MMIO_inst_pwm_0_width_read(void) {
	return csr_read_simple(CSR_BASE + 0x10L);
}
static inline void MMIO_inst_pwm_0_width_write(uint32_t v) {
	csr_write_simple(v, CSR_BASE + 0x10L);
}
#define CSR_MMIO_INST_STEPGEN_STEPLEN_ADDR (CSR_BASE + 0x14L)
#define CSR_MMIO_INST_STEPGEN_STEPLEN_SIZE 1
static inline uint32_t MMIO_inst_stepgen_steplen_read(void) {
	return csr_read_simple(CSR_BASE + 0x14L);
}
static inline void MMIO_inst_stepgen_steplen_write(uint32_t v) {
	csr_write_simple(v, CSR_BASE + 0x14L);
}
#define CSR_MMIO_INST_STEPGEN_DIR_HOLD_TIME_ADDR (CSR_BASE + 0x18L)
#define CSR_MMIO_INST_STEPGEN_DIR_HOLD_TIME_SIZE 1
static inline uint32_t MMIO_inst_stepgen_dir_hold_time_read(void) {
	return csr_read_simple(CSR_BASE + 0x18L);
}
static inline void MMIO_inst_stepgen_dir_hold_time_write(uint32_t v) {
	csr_write_simple(v, CSR_BASE + 0x18L);
}
#define CSR_MMIO_INST_STEPGEN_DIR_SETUP_TIME_ADDR (CSR_BASE + 0x1cL)
#define CSR_MMIO_INST_STEPGEN_DIR_SETUP_TIME_SIZE 1
static inline uint32_t MMIO_inst_stepgen_dir_setup_time_read(void) {
	return csr_read_simple(CSR_BASE + 0x1cL);
}
static inline void MMIO_inst_stepgen_dir_setup_time_write(uint32_t v) {
	csr_write_simple(v, CSR_BASE + 0x1cL);
}
#define CSR_MMIO_INST_STEPGEN_APPLY_TIME_ADDR (CSR_BASE + 0x20L)
#define CSR_MMIO_INST_STEPGEN_APPLY_TIME_SIZE 2
static inline uint64_t MMIO_inst_stepgen_apply_time_read(void) {
	uint64_t r = csr_read_simple(CSR_BASE + 32L);
	r <<= 32;
	r |= csr_read_simple(CSR_BASE + 0x24L);
	return r;
}
static inline void MMIO_inst_stepgen_apply_time_write(uint64_t v) {
	csr_write_simple(v >> 32, CSR_BASE + 0x20L);
	csr_write_simple(v, CSR_BASE + 0x24L);
}
#define CSR_MMIO_INST_STEPGEN_0_SPEED_ADDR (CSR_BASE + 0x28L)
#define CSR_MMIO_INST_STEPGEN_0_SPEED_SIZE 1
static inline uint32_t MMIO_inst_stepgen_0_speed_read(void) {
	return csr_read_simple(CSR_BASE + 0x28L);
}
static inline void MMIO_inst_stepgen_0_speed_write(uint32_t v) {
	csr_write_simple(v, CSR_BASE + 0x28L);
}
#define CSR_MMIO_INST_STEPGEN_0_MAX_ACCELERATION_ADDR (CSR_BASE + 0x2cL)
#define CSR_MMIO_INST_STEPGEN_0_MAX_ACCELERATION_SIZE 1
static inline uint32_t MMIO_inst_stepgen_0_max_acceleration_read(void) {
	return csr_read_simple(CSR_BASE + 0x2cL);
}
static inline void MMIO_inst_stepgen_0_max_acceleration_write(uint32_t v) {
	csr_write_simple(v, CSR_BASE + 0x2cL);
}
#define CSR_MMIO_INST_WATCHDOG_HAS_BITTEN_ADDR (CSR_BASE + 0x30L)
#define CSR_MMIO_INST_WATCHDOG_HAS_BITTEN_SIZE 1
static inline uint32_t MMIO_inst_watchdog_has_bitten_read(void) {
	return csr_read_simple(CSR_BASE + 0x30L);
}
#define CSR_MMIO_INST_WALL_CLOCK_ADDR (CSR_BASE + 0x34L)
#define CSR_MMIO_INST_WALL_CLOCK_SIZE 2
static inline uint64_t MMIO_inst_wall_clock_read(void) {
	uint64_t r = csr_read_simple(CSR_BASE + 52L);
	r <<= 32;
	r |= csr_read_simple(CSR_BASE + 0x38L);
	return r;
}
#define CSR_MMIO_INST_GPIO_IN_ADDR (CSR_BASE + 0x3cL)
#define CSR_MMIO_INST_GPIO_IN_SIZE 1
static inline uint32_t MMIO_inst_gpio_in_read(void) {
	return csr_read_simple(CSR_BASE + 0x3cL);
}
#define CSR_MMIO_INST_STEPGEN_0_POSITION_ADDR (CSR_BASE + 0x40L)
#define CSR_MMIO_INST_STEPGEN_0_POSITION_SIZE 2
static inline uint64_t MMIO_inst_stepgen_0_position_read(void) {
	uint64_t r = csr_read_simple(CSR_BASE + 64L);
	r <<= 32;
	r |= csr_read_simple(CSR_BASE + 0x44L);
	return r;
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

#endif
