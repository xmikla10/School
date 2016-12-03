<?php 
/*****************Zaciatok******************/
/**
*
* @author      Peter Miklánek xmikla10
*
*/


//hlavicka trocha blbne, pri dokonceni skontrolovat


/*..................JSON..................*/
class json
{	
	public static function JSONdecode($retazec)
	{
		if(preg_match('//u', $retazec))
		{
				$pom2 = json_decode($retazec);
				return $pom2;
		}
		else
		{
				echo "Nespravne UTF-8 v JSON\n";
				die(4);	
		}
	}
}
/*..................JSON..................*/
/*..................XML..................*/
class xml
{
	private $data = NULL;
	private $meno = NULL;
	private $attributes = array();
	private $print = 0;
	private $pole = FALSE;
	private $subst = TRUE;

	//The __construct() function creates a new SimpleXMLElement object.
	public function __construct($meno, $data, $print = 0)
	{
		// tu vyuzivam pomocne funkcie is_object, is_array, ktoré mi zjednodusia pracu
		if(($meno) == null)
				$print -= 1;

		$this->print = $print;
		$this->name = $meno;
		if(is_object($data))
		{
				$this->data = array();
				foreach($data as $key => $val)
						$this->data[] = new xml($key, $val, $print+1);
		}
		elseif(is_array($data))
		{
			$this->pole = true;
			$pom1= arg::F_start();
			$pom2= strlen($pom1+ count($data)-1);

			foreach($data as $key => $val)
			{
				$this->data[] = new xml(arg::F_item(), $val, $print+2);
				if(arg::$index_items)
						$this->data[ count($this->data)-1 ]->setAttribute('index', $pom1);
					$pom1++;
			}
		}
		else
				$this->data = $data;
	}
	public function output()
	  {
	    $result = '';
	    if(is_array($this->data))
	    {
	      if(($this->name) !== null)
	      {
	        $result .= $this->F_print().'<'.$this->escapeTagName($this->name).$this->getAttributes().'>'."\n";
	      }
	      if($this->pole)
	      { // pri zadani -a alebo --array-size doplni size u pole
	        $result .= $this->F_print($this->print+1).'<'.$this->escapeTagName(arg::F_array()).(arg::$array_size ? ' size="'.count($this->data).'"' : '').'>'."\n";
	      }
	      foreach($this->data as $dat)
	          $result .= $dat->output();
	      if($this->pole)
	      {
	        $result .= $this->F_print($this->print+1).'</'.$this->escapeTagName(arg::F_array()).'>'."\n";
	      }
	      if(!is_null($this->name))
	      {
	        $result .= $this->F_print().'</'.$this->escapeTagName($this->name).'>'."\n";
	      }
	    }
	    else
	    {
	      if( (is_null($this->data) OR is_bool($this->data)) AND arg::$number)
	      {
	        $result .= $this->F_print().'<'.$this->escapeTagName($this->name).$this->getAttributes().'>'."\n";
	        $result .= $this->F_print($this->print+1).'<'.$this->escapeValue($this->data).' />'."\n";
	        $result .= $this->F_print().'</'.$this->escapeTagName($this->name).'>'."\n";
	      }
			elseif( ((is_float($this->data) OR is_int($this->data) ) AND arguments::$number) OR (is_string($this->data)  AND arguments::$string) )
			{ // Number or string
				$result .= $this->indent().'<'.$this->escapeTagName($this->name).$this->getAttributes().'>'.$this->escapeValue($this->data).'</'.$this->escapeTagName($this->name).'>'."\n";
			}
			else
			{
				$result .= $this->indent().'<'.$this->escapeTagName($this->name).$this->getAttributes().' value="'.$this->escapeValue($this->data).'" />'."\n";
			}



	      /*elseif(is_string($this->data)) 
	      {
	        $result .= $this->F_print().'<'.$this->escapeTagName($this->meno).$this->getAttributes().'value="'.$this->escapeValue($this->data).'" />'."\n";
	      }
	      else
	      { 
	        $result .= $this->F_print().'<'.$this->escapeTagName($this->name).$this->getAttributes().'>'.$this->escapeValue($this->data).'</'.$this->escapeTagName($this->name).'>'."\n";
	      }*/
	    }
	    return $result;
	  }

	//Funkcia nahradenie mena xml elementov
	public function setSubstitution($value)
	{
			$this->subst = $value;
	}
	//Funkcia na nastavenie Atributov
	public function setAttribute($meno, $value)
	{
			$this->attributes[$meno] = $value;
	}
	public function getAttributes()
	{
			if(count($this->attributes) != 0)
			{
				$attr = '';
				foreach($this->attributes as $key => $val)
				{
					$attr .= ' '.$key.'="'.$val.'"';
				}
				return $attr;
			}
			else return '';
	}	
	private function F_print($num = NULL)
	{
		if(($num) === null)
				$num = $this->print;
		//str_pad returns the input string padded on the left, the right, or both sides to the specified padding length
		return str_pad('', $num, "\t");
	}
	private function escapeTagName($meno)
	{
		if($this->subst)
		{
			//preg_replace() returns an array if the subject parameter is an array, or a string otherwise
			$meno = preg_replace('/(\?|<|>|\(|\)|{|}|[|]|,|;|\*|%|!|"|&|#|\+|=|~|\`|@|\/|\s|^-|^[0-9])/', arg::F_substring(), $meno);
		}
		if(preg_match('/^(-|[0-9]+)/', $meno, $out))
		{
			echo "Error nejaky";
			exit(51);
		}
		return $meno;
	}
	private function escapeValue($value)
	{
		if(($value) === null)
				return 'null';
		else if(is_float($value))
				return floor($value);
		else if(is_bool($value))
				return $value ? 'true' : 'false';
		else
				return arg::$translate ? htmlspecialchars($value) : str_replace('"','&quot;',$value);
	}	
}
/*..................XML..................*/
/*..................ARG..................*/
class arg 
{
	static $help = FALSE; //--help

	static $inputFile = NULL; //--input=filename
	const std_inputFile = 'php://stdin';

	static $outputFile = NULL; //--output=filename
	const std_outputFile = 'php://stdout';

	static $header = FALSE; //-n
	static $substring = NULL; //-h=subst
	const constant_substring = '-';

	static $root_element = NULL; //-r=root-element
	static $array = NULL; //--array-name=array-element
	const constant_array = 'array';

	static $item = NULL; //--item-name=item-element
	const constant_item = 'item';

	static $string = FALSE; //-s
	static $number = FALSE; //-i
	static $boolean = FALSE; //-l
	static $translate = FALSE; //-c 
	static $array_size = FALSE;//-a, --array-size
	static $index_items = FALSE; //-t, --index-items

	static $start = NULL; //--start=n
	const constant_start = 1;	

	public static function spracovanie_vstupu(array $argv)
	{
		//bere argumenty az od argv[1]
		unset($argv[0]);
		if(count($argv) == 0)
		{
			echo "Any arguments, for show help : --help\n";
			exit();
		}
		foreach ($argv as $vstup) 
		{
			/* Pokud chceme přistupovat ke statickým metodám 
			či vlastnostem uvnitř třídy, používáme místo jména 
			třídy klíčová slova self:: */
			if($vstup === '--help')
			{
				if(count($argv) > 1)
				{
						echo "Argument --help can be just alone\n"; die(1);	
				}
				else self::$help = TRUE;
			}
			elseif(substr($vstup, 0, 8) === '--input=')
			{
					if((self::$inputFile)  !== null)	
					{
						 	echo "Already specified ( --input= )\n"; die(1);
					}
					self::$inputFile = substr($vstup, 8);
			}
			elseif(substr($vstup, 0, 9) === '--output=')
			{
					if((self::$outputFile)  !== null)	
					{
						 	echo "Already specified ( --output= )\n"; die(1);
					}
					self::$outputFile = substr($vstup, 9);
			}
			elseif($vstup === '-n')
			{
					if(self::$header)	
					{
						 	echo "Already specified ( -n )\n"; die(1);
					}
					self::$header = TRUE;
			}
			elseif(substr($vstup, 0, 3) === '-h=')
			{
					if((self::$substring)  !== null)	
					{
						 	echo "Already specified ( -h= )\n"; die(1);
					}
					self::$substring = substr($vstup, 3);
			}
			elseif(substr($vstup, 0, 3) === '-r=')
			{
					if((self::$root_element)  !== null)	
					{
						 	echo "Already specified ( -r= )\n"; die(1);
					}
					self::$root_element = substr($vstup, 3);
			}

			elseif(substr($vstup, 0, 13) === '--array-name=')
			{
					if((self::$array)  !== null)	
					{
						 	echo "Already specified ( --array-name= )\n"; die(1);
					}
					self::$array = substr($vstup, 13);
			}

			elseif(substr($vstup, 0, 12) === '--item-name=')
			{
					if((self::$item)  !== null)	
					{
						 	echo "Already specified ( -item-name= )\n"; die(1);
					}
					self::$item = substr($vstup, 12);
			}

			elseif($vstup === '-s')
			{
					if(self::$string)	
					{
						 	echo "Already specified ( -s )\n"; die(1);
					}
					self::$string = TRUE;
			}

			elseif($vstup === '-i')
			{
					if(self::$number)	
					{
						 	echo "Already specified ( -i )\n"; die(1);
					}
					self::$number = TRUE;
			}

			elseif($vstup === '-l')
			{
					if(self::$boolean)	
					{
						 	echo "Already specified ( -l )\n"; die(1);
					}
					self::$boolean = TRUE;
			}

			elseif($vstup === '-c')
			{
					if(self::$translate)	
					{
						 	echo "Already specified ( -c )\n"; die(1);
					}
					self::$translate = TRUE;
			}

			elseif($vstup === '-a' || $vstup === '--array-size')
			{
					if(self::$array_size)	
					{
						 	echo "Already specified ( -a, --array-size )\n"; die(1);
					}
					self::$array_size = TRUE;
			}

			elseif($vstup === '-t' || $vstup === '--index-items')
			{
					if(self::$index_items)	
					{
						 	echo "Already specified ( -t, --index-items )\n"; die(1);
					}
					self::$index_items = TRUE;
			}

			elseif(substr($vstup, 0, 8) === '--start=')
			{
					if((self::$start)  !== null)	
					{
						 	echo "Already specified ( --start= )\n"; die(1);
					}
					self::$start = substr($vstup, 8);
					//mozno bude treba este nejako osetrit pretoze by
					//to malo byt vzdy s parametrom --index-items
			}
			else
			{
					echo "Unknown argument $vstup \n";				
					die (1);
			}
		}
		if(((self::$start)!== null) AND !self::$index_items)
		{
			 	echo "--start= without --index-items \n";
			 	die(1);
		}

		try
		{
			if(self::$root_element)
			{
				$x = new xml(self::$root_element, 0);
				$x->setSubstitution(FALSE);
				$x->output();
			}
			$pom = new xml(self::F_array(), 0);
			$pom->setSubstitution(FALSE);
			$pom->output();

			$pom = new xml(self::F_item(), 0);
			$pom->setSubstitution(FALSE);
			$pom->output();

		}
		catch(specified $code) 
		{
				echo $code;
    			die($code);
		}
	}
	//funkcia na overenie input suboru
	public static function F_input_file()
	{
		if((self::$inputFile) == null) 
				return self::std_inputFile;	
		else return self::$inputFile;
	}
	//funkcia na overenie output suboru
	public static function F_output_file()
	{
		if((self::$outputFile) == null)
				return self::std_outputFile;
		else return self::$outputFile;
	}
	public static function F_array()
	{
		if((self::$array) == null)
				return self::constant_array;
		else return self::$array;
	}
	public static function F_item()
	{
		if((self::$item) == null)
				return self::constant_item;
		else return self::$item;
	}
	public static function F_start()
	{
		if((self::$start) == null)
				return self::constant_start;
		else return self::$start;
	}
	public static function F_substring()
	{
		if((self::$substring) == null)
			return self::constant_substring;
		else return self::$substring;
	}				
}
/*..................ARG..................*/
/*..................FILE..................*/
class spracovanie_suboru
{
	public static function citanie($meno_suboru)
	{
		/*Funkce file_get_contents se používá pro načtení
		  souboru a jeho obsahu vložení do proměnné*/
		if (!is_readable($meno_suboru) AND $meno_suboru != arg::std_inputFile)
		{
			 	echo "Chyba, neda sa otvorit\n";
			 	die(2);
		} 

		$pom = file_get_contents($meno_suboru);
		return $pom;
	}

	public static function zapis($meno_suboru, $hlavicka2)
	{
		/*Funkce file_put_contents je jedno z nejspolehlivějších
		 funkcí pro ukládání textových souborů */
		$pom = file_put_contents($meno_suboru, $hlavicka2);
		if ($pom == FALSE)
		{
				echo "Error ... doplnit\n";
				die(2);
		}
		//treba este osetrit !
	}
}
/*..................FILE..................*/
/*****TRY mi sluzi ako "main"******/
try
{
	arg::spracovanie_vstupu($argv);
	$hlavicka = '<?xml version="1.0" encoding="UTF-8"?>	'."\n";

	if(arg::$help)
	{
		show_help();		
		die(0);
	}

	$data = json::JSONdecode( spracovanie_suboru::citanie( arg::F_input_file()));

	if(arg::$header)
	{
		$hlavicka = '';
	}

	$pom = new xml(arg::$root_element, $data);

	spracovanie_suboru::zapis(arg::F_output_file(), $hlavicka.$pom->output());
}
/* Catch vobec nepouzivam ale try ano tak som to jednoducho vyuzil */
catch(nothing $ahoj) 
{
		echo "$ahoj";
}

class nothing
{
	public function returnNothing($ahoj)
	{
		return "Nothing\n";
	}
}
/*.........show help zobrazi napovedu pri zadani --help..........*/
function show_help()
{
	echo "
	--help viz společné zadání všech úloh
	--input=filename zadaný vstupní JSON soubor v kódování UTF-8
	--output=filename textový výstupní XML soubor v kódování UTF-8 s obsahem převedeným
		ze vstupního souboru
	-h=subst ve jméně elementu odvozeném z dvojice jméno-hodnota nahraďte každý nepovolený
		znak ve jméně XML značky řetězcem subst. Implicitně (i při nezadaném parametru -h) uvažujte
		nahrazování znakem pomlčka (-). Vznikne-li po nahrazení invalidní jméno XML elementu,
		skončete s chybou a návratovým kódem 51.
	-n negenerovat XML hlavičku1 na výstup skriptu (vhodné například v případě kombinování
		více výsledků)
	-r=root-element jméno párového kořenového elementu obalujícího výsledek. Pokud nebude
		zadán, tak se výsledek neobaluje kořenovým elementem, ač to potenciálně porušuje validitu
		XML (skript neskončí s chybou). Zadání řetězce root-element vedoucího na nevalidní XML
		značku ukončí skript s chybou a návratovým kódem 50 (nevalidní znaky nenahrazujte).
	--array-name=array-element tento parametr umožní přejmenovat element obalující pole
		z implicitní hodnoty array na array-element. Zadání řetězce array-element vedoucího na
		nevalidní XML značku ukončí skript s chybou a návratovým kódem 50 (nevalidní znaky nenahrazujte).
	--item-name=item-element analogicky, tímto parametrem lze změnit jméno elementu pro
		prvky pole (implicitní hodnota je item). Zadání řetězce item-element vedoucího na nevalidní
		XML značku ukončí skript s chybou a návratovým kódem 50 (nevalidní znaky nenahrazujte).
	-s hodnoty (v dvojici i v poli) typu string budou transformovány na textové elementy místo
		atributů.
	-i hodnoty (v dvojici i v poli) typu number budou transformovány na textové elementy místo
		atributů.
	-l hodnoty literálů (true, false, null) budou transformovány na elementy <true/>,
		<false/> a <null/> místo na atributy.
	-c tento přepínač oproti implicitnímu chování aktivuje překlad problematických znaků. Pro
		XML problematické znaky s UTF-8 kódem menším jak 128 ve vstupních řetězcových hodnotách
		(ve dvojicích i polích) konvertujte na odpovídající zápisy v XML pomocí metaznaku & (např.
		&amp;, &lt;, &gt; atd.). Ostatní problematické znaky konvertovat nemusíte.
	-a, --array-size u pole bude doplněn atribut size s uvedením počtu prvků v tomto poli.
	-t, --index-items ke každému prvku pole bude přidán atribut index s určením indexu prvku v tomto poli (číslování začíná od 1, pokud není parametrem --start určeno jinak).
	--start=n inicializace inkrementálního čitače pro indexaci prvků pole (nutno kombinovat
		s parametrem --index-items, jinak chyba s návratovým kódem 1) na zadané kladné celé číslo
		n včetně nuly (implicitně n = 1).\n ";
}

/*****************KONIEC******************/
?>