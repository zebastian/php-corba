<?php
$ior = "IOR:000000000000000f49444c3a52616e646f6d3a312e3000000000000100000000000000500001000000000016706c616e7874792e6473672e63732e7463642e69650006220000002c3a5c706c616e7874792e6473672e63732e7463642e69653a52616e646f6d3a303a3a49523a52616e646f6d00";
$obj = new UniverseObject($ior);
$value = $obj->lrand48();
echo "Random value retrieved: $value\n";
?>