# Autogenerated by LiteX / git: 82daa48e
set -e
yosys -l colorlight_5a_75b.rpt colorlight_5a_75b.ys
nextpnr-ecp5 --json colorlight_5a_75b.json --lpf colorlight_5a_75b.lpf --textcfg colorlight_5a_75b.config      --25k --package CABGA256 --speed 6 --timing-allow-fail  --seed 1
ecppack colorlight_5a_75b.config --svf colorlight_5a_75b.svf --bit colorlight_5a_75b.bit --bootaddr 0  
