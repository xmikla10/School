<?php
/*
 * #JSN:xsedla0m
 * Autor: Michal Sedlák (xsedla0m@fit.vutbr.cz)
 * IPP 2016 JSN2XML
 */

error_reporting(E_ALL & ~E_NOTICE);

// getopt
$shortopts = "";
$shortopts .= "h:"; // -h=subst
$shortopts .= "r:"; // -r=root-element
$shortopts .= "n";  // -n 
$shortopts .= "s";
$shortopts .= "i";
$shortopts .= "l";
$shortopts .= "c";
$shortopts .= "a"; // --array-size
$shortopts .= "t"; // --index-items


$longopts = array(
    "help",        // --help   
    "array-size",  // --array-size  
    "index-items", // --index-items  
    "input:",      // vstupny soubor
    "output:",     // vystupny soubor
    "array-name:", // --array-name=array-element
    "item-name:",  // --item-name=item-element
    "start:",      // --start=n (int)
);
$options = getopt($shortopts, $longopts);

if ($options["n"] === false) { // parametere
    $n = true;
}
$s = false;
if ($options["s"] === false) {
    $s = true;
}
$i = false;
if ($options["i"] === false) {
    $i = true;
}
$l = false;
if ($options["l"] === false) {
    $l = true;
}
$c = false;
if ($options["c"] === false) {
    $c = true;
}
$a = false;
if ($options["a"] === false || $options["array-size"] === false) {
    $a = true;
}
$t = false;
if ($options["t"] === false || $options["index-items"] === false) {
    $t = true;
}
$help = false;
if ($options["help"] === false) {
    $help = true;
}
$h = "-";
if ($options["h"]) {
    $h = $options["h"];
}
$r = false;
if ($options["r"]) {
    if (preg_match('/^[\p{L}_][\p{L}\d-._]*$/u', $options["r"]) == 1) {
        if (preg_match("/^[x][m][l]/i", $options["r"]) == 0) {
            $r = $options["r"];
        } else {
            echo "zly parameter r";
            exit(50);
        }
    } else {
        echo "zly parameter array-name";
        exit(50);
    }
}

$arrayName = "array";
if ($options["array-name"]) {
    if (preg_match('/^[\p{L}_][\p{L}\d-._]*$/u', $options["array-name"]) == 1) {
        if (preg_match("/^[x][m][l]/i", $options["array-name"]) == 0) {
            $arrayName = $options["array-name"];
        } else {
            echo "Zlý parameter array-name";
            exit(50);
        }
    } else {
        echo "Zlý parameter array-name";
        exit(50);
    }
}

$itemName = "item";
if ($options["item-name"]) {
    if (preg_match('/^[\p{L}_][\p{L}\d-._]*$/u', $options["item-name"]) == 1) {
        if (preg_match("/^[x][m][l]/i", $options["item-name"]) == 0) {
            $itemName = $options["item-name"];
        } else {
            echo "Zlý parameter item-name";
            exit(50);
        }
    } else {
        echo "Zlý parameter array-name";
        exit(50);
    }
}

$start = 1;
if (is_string($options["start"])) {
    if ($t === true) {

        if (is_numeric($options["start"])) {
            $start = $options["start"];
        } else {
            echo "Zlý parameter -start";
            exit(1);
        }
    } else {
        echo "Chýba parameter -t pri použití -start";
        exit(1);
    }
}

// Napoveda
if ($help === true) { // Ak je zadany jediny argument --help
    if (count($argv) === 2) {
        echo "


Nápoveda JSON2XML 

--input=filename 	zadaný vstupný JSON subor v kódování UTF-8
                            
--output=filename 	textový výstupný XML súbor v kódování UTF-8 s obsahom prevedením
			zo vstupného súboru
							
-h=subst 	z elementu odvodené z dvojice méno-hodnota nahradzuje každý 
		nepovolený znak v mene XML značky reťazcom subst. Implicitná pomlčka: (-)
							
-n 	negenerovať XML hlavičku na výstup skriptu

-r=root-element   meno párového koreňového elementu obalujúci výsledok

--array-name=array-element 	tento parameter umožní premenovať element obaľujúci pole
				z implicitnej hodnoty array na array-element.
							
--item-name=item-element analogicky, tímto parametrom ide změniť méno elementu pre
                         prvky pola (implicitná hodnota je item)
							
-s 	hodnoty (v dvojici i v poli) typu string budú transformované na 
	textové elementy miesto atributov.
							
-i 	hodnoty (v dvojici i v poli) typu number budú transformováne na 
	textové elementy miesto atributov.
							
-l 	hodnoty literálov (true, false, null) budú transformované na elementy 
	<true/>, <false/> a <null/> miesto na atributy.

-c 	aktivuje preklad problematických znakov.

-a, --array-size 	v poli bude doplnený atribut size s uvedeným počtom prvkov v tomto poli.

-t, --index-items 	ku každému prvku poľa bude pridaný atribut index s určeným indexom prvku
                   	 v tomto poly (číslováníie začíná od 1, pokiaľ není parameterom --start určené inak).
							
--start=n   inicializacácia inkrementálného čítača pre indexaci prvkov poľa (nutno kombinovat
            s parameterom --index-items) na zadané kladné celé číslo n aj s nulou (implicitne
            n = 1).
        ";

        exit(0);
    } else { //nieje jediny konec chyba: 1 
        echo "Veľa argumentov pri --help\n";
        exit(1);
    }
}
//Nezadaný parameter n 

if ((count($argv) - 1) !== count($options)) { //Kontrola zpracovaných argumentov
    echo ("Argumenty boli nesprávne zadané\n");
    exit(1);
}
$vysledok = "";

if ($n === false) { //Generovanie hlavičky
    $vysledok .= "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
}
//Vstupny súbor
if ($options["input"]) {

    $inputFile = file_get_contents(realpath($options["input"])) or exit(2);
} else {
    $inputFile = file_get_contents("php://stdin", "r") or exit(2);
}

function traverseArray(&$array) { //Nahradzuje true null a false za odpovedajúce stringy
    foreach ($array as $key => &$value) {
        if (is_array($value)) {
            traverseArray($value);
        } else {

            if ($value === true) {
                $array[$key] = "true";
            }
            if ($value === null) {
                $array[$key] = "null";
            }
            if ($value === false) {
                $array[$key] = "false";
            }
        }
    }
}

//Nahradzujem akykolvek nevalidny XML znak za argument -h
function nahrad($key) {
    global $h;
    //$kluc = preg_replace("/[^\w.-]/u", $h, $key); nefunguje na merlinovi, nechapem preco
    $kluc = preg_replace("/[^a-zA-Z0-9úďěščřžýáíéůňóťÚĎĚŠČŘŽÝÁÍÉŇÓŤ_.-]/", $h, $key);
    return $kluc;
}

function recurseJson2XML(&$json, $root = false) {
    global $arrayName, $itemName, $r, $a, $t, $start, $c, $i, $s, $l;
    $element = '';
    if ($json[0]) {
        if ($a === true) { //Rieším prepinac -a alebo -array-size
            $element .= "<{$arrayName} size=\"" . count($json) . "\">\n";
        } else {
            $element .= "<{$arrayName}>\n";
        }
    }
    foreach ($json as $key => &$value) {

        if ($c === true) { //Preklad problematickych znaku
            $value = str_replace("&", "&amp;", $value);
            $value = str_replace("<", "&lt;", $value);
            $value = str_replace(">", "&gt;", $value);
            $value = str_replace("'", "&apos;", $value);
            $value = str_replace("\"", "&quot;", $value);
        }
        if (is_float($value)) { // k je cislo float zaokrouhlujem dolu
            $value = floor($value);
        }

        if (is_numeric($key)) {

            if (is_array($value)) {
                if ($t === true) {
                    $element .= "<" . $itemName . " index=\"" . ($key + $start) . "\">\n";
                } else {
                    $element .= "<{$itemName}>\n";
                }
            } else {
                if (is_numeric($value) && $i === true) {
                    if ($t === true) {

                        $element .= "<" . $itemName . " index=\"" . ($key + $start) . "\">\n";
                    } else {

                        $element .= "<{$itemName}>\n";
                    }
                } else {
                    if ($t === true) {
                        if (($value == "true" || $value == "false" || $value == "null") && $l === true) {
                            $element .= "<" . $itemName . " index=\"" . ($key + $start) . "\">\n<{$value}/>\n";
                        } else {
                            $element .= "<" . $itemName . " index=\"" . ($key + $start) . "\" value=\"" . $value . "\"/>\n";
                        }
                    } else {
                        if (($value == "true" || $value == "false" || $value == "null") && $l === true) {
                            $element .= "<{$itemName}>\n<{$value}/>\n";
                        } else {
                            $element .= "<{$itemName} value=\"{$value}\"/>\n";
                        }
                    }
                }
            }
        } else if (is_string($key)) {
            $key = nahrad($key);
            if (preg_match('/^[\p{L}_][\p{L}\d-._]*$/u', $key) != 1) {
                echo "Nesprávne meno elementu";
                exit(51);
            } else {
                if (preg_match("/^[x][m][l]/i", $key) != 0) {
                    echo "Nesprávne meno elementu";
                    exit(51);
                }
            }
            if (is_array($value)) {
                $element .= "<{$key}>\n";
            } else {

                if ($s === true && is_string($value)) {
                    if (($value == "true" || $value == "false" || $value == "null") && $l === true) {
                        $element .= "<{$key}>\n<{$value}/>\n";
                    } else if ($value == "true" || $value == "false" || $value == "null") {
                        $element .= "<{$key} value=\"{$value}\"/>\n";
                    } else {
                        $element .= "<{$key}>\n{$value}\n";
                    }
                } else {
                    if (($value == "true" || $value == "false" || $value == "null") && $l === true) {
                        $element .= "<{$key}>\n<{$value}/>\n";
                    } else {
                        $element .= "<{$key} value=\"{$value}\"/>\n";
                    }
                }
            }
        } else {
            $element .= "<{$key}>\n";
        }

        if (is_array($value)) {

            $element .= recurseJson2XML($value);
        } else {  // Tak sa nestane nic 
        }
           
        if (is_numeric($key)) {

            if (!is_numeric($value)) {
                if ($l === true) {
                    $element .= "</{$itemName}>\n";
                } else {
                    if (!($value == "true" || $value == "false" || $value == "null")) {
                        $element .= "</{$itemName}>\n";
                    }
                }
            }
            if (is_numeric($value) && $i === true) {
                $element .= "{$value}\n</{$itemName}>\n";
            }
        }
        if (is_string($key)) {
            if (is_array($value)) {
                $element .= "</{$key}>\n";
            } elseif ($s === true && is_string($value)) {
                if (!($value == "true" || $value == "false" || $value == "null")) {

                    $element .= "</{$key}>\n";
                }
                if (($value == "true" || $value == "false" || $value == "null") && $l === true) {
                    $element .= "</{$key}>\n";
                }
            } elseif (($value == "true" || $value == "false" || $value == "null") && $l === true) {
                $element .= "</{$key}>\n";
            } else {
                
            }
        }
    }
    if ($json[0]) {
        $element .= "</{$arrayName}>\n";
    }
    if ($root) {
        if ($r === false) {
            $element = "{$element}";
        } else {
            $element = "<{$r}>\n{$element}</{$r}>\n";
        }
    }
    return $element;
}

// Pole obsahujuce zpracovany JSON 
$json = json_decode($inputFile, true);

// Ak je vstupny subor JSON valid
if (json_last_error() === 0) {
    traverseArray($json);
    $vysledok .= recurseJson2XML($json, true);
} else {
    echo "Nesprávny JSON";
    exit(4);
}

// Vystupny subor
if ($options["output"]) {
    file_put_contents($options["output"]) $vysledok) or exit(3); 
// urobit UTF mozna "\xEF\xBB\xBF". $inputFile
} else {
    file_put_contents('php://stdout', $vysledok) or exit(2);
}

exit(0);

