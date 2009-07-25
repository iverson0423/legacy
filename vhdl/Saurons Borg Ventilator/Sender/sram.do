onerror {resume}
quietly WaveActivateNextPane {} 0
add wave -noupdate -format Logic -radix unsigned /vtb_ram_control_3_vhd/clk50
add wave -noupdate -format Logic -radix unsigned /vtb_ram_control_3_vhd/clk100
add wave -noupdate -format Literal -radix unsigned /vtb_ram_control_3_vhd/sram_adr
add wave -noupdate -format Literal -radix unsigned /vtb_ram_control_3_vhd/sram_1_io
add wave -noupdate -format Literal -radix unsigned /vtb_ram_control_3_vhd/sram_2_io
add wave -noupdate -format Logic -radix unsigned /vtb_ram_control_3_vhd/sram_we
add wave -noupdate -format Logic -radix unsigned /vtb_ram_control_3_vhd/sram_oe
add wave -noupdate -format Literal -radix unsigned /vtb_ram_control_3_vhd/sram_read
add wave -noupdate -format Literal -radix unsigned /vtb_ram_control_3_vhd/sram_pos
add wave -noupdate -format Literal -radix unsigned /vtb_ram_control_3_vhd/winkel
add wave -noupdate -format Literal -radix unsigned /vtb_ram_control_3_vhd/winkel_diag
add wave -noupdate -format Logic /vtb_ram_control_3_vhd/sram_1_ce
add wave -noupdate -format Logic /vtb_ram_control_3_vhd/sram_2_ce
TreeUpdate [SetDefaultTree]
WaveRestoreCursors {{Cursor 1} {1295250630 ps} 0}
configure wave -namecolwidth 290
configure wave -valuecolwidth 100
configure wave -justifyvalue left
configure wave -signalnamewidth 0
configure wave -snapdistance 10
configure wave -datasetprefix 0
configure wave -rowmargin 4
configure wave -childrowmargin 2
configure wave -gridoffset 0
configure wave -gridperiod 1
configure wave -griddelta 40
configure wave -timeline 0
configure wave -timelineunits ps
update
WaveRestoreZoom {1421396172 ps} {1421639561 ps}