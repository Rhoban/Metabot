<?php

$nops = array(400, 450, 800, 850);
$magic_constant = 45;

foreach ($nops as $nop) {
    echo "#define NOPS_$nop \\\n";
    echo "  asm volatile(\"";
    $n = round($nop/$magic_constant);
    for ($k=0; $k<$n; $k++) echo "nop;";
    echo "\");\n\n";
}
