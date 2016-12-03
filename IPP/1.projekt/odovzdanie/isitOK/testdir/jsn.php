<?php 
/*
#JSN:xmikla10
*
*       Peter Miklánek, xmikla10
*        		16.3.2016
*
*/
/*****************START******************/

main($argv);

/*..................MAIN.................*/
/*Funkcia ktora mi sluzi ako "main" a je zavolana hned nazaciatku*/
function main(array $argv)
{
	arg::arguments($argv);
	$hlavicka = '<?xml version="1.0" encoding="UTF-8"?>'."\n";
	//ak bol na vstupe zadany --help, zavola sa funkcia, ktora help vypise
	if(arg::$help)
	{
		show_help();		
		exit();
	}
	$data = json::JSONdecode( file::cteni( arg::F_input_file()));
	//ak je na vstupe zadane -n, hlavicka sa nezapisuje
	if(arg::$header)
			$hlavicka = false;

	$pom = new xml(arg::$root_element, $data);
	//zapise do vysledneho XML suboru
	file::zapis(arg::F_output_file(), $hlavicka.$pom->zapisDoXML());
}
/*..................MAIN.................*/

/*..................FILE..................*/
/*Trieda urcena na cteni a zapis dat dat*/
class file
{
	public function cteni($meno_suboru)
	{ //pomocna funkcia vyuzivajuca funkciu file_get_contents
		/*Funkce file_get_contents se používá pro načtení
		  souboru a jeho obsahu vložení do proměnné*/
		if (!is_readable($meno_suboru))
		{
			if($meno_suboru != arg::std_inputFile)
			 	exit(2);
		} 

		$pom = file_get_contents($meno_suboru);
		return $pom;
	}
	public function zapis($meno_suboru, $hlavicka)
	{ //pomocna funkcia vyuzivajuca funkciu file_put_contents
		/*Funkce file_put_contents je jedno z nejspolehlivějších
		 funkcí pro ukládání textových souborů */
		if ((file_put_contents($meno_suboru, $hlavicka)) == false)	
			exit(3);
		else
			$pom = file_put_contents($meno_suboru, $hlavicka);
	}
}
/*..................FILE..................*/

/*..................ARG..................*/
/* Trieda urcena na spracovanie vstupnych argumentov */
class arg 
{
	static $help; //--help

	static $inputFile; //--input=filename
	const std_inputFile = 'php://stdin';

	static $outputFile; //--output=filename
	const std_outputFile = 'php://stdout';

	static $header; //-n
	static $subst; //-h=subst
	const constant_substring = '-';

	static $array; //--array-name=array-element
	const constant_array = 'array';

	static $item; //--item-name=item-element
	const constant_item = 'item';

	static $root_element; //-r=root-element
	static $string; //-s
	static $number; //-i
	static $boolean; //-l
	static $convert; //-c 
	static $array_size;//-a, --array-size
	static $index_items; //-t, --index-items

	static $start; //--start=n
	const constant_start = 1;	

	/*Funkcia na overenie input suboru*/
	public function F_input_file()
	{
		if((self::$inputFile) == null) 
				return self::std_inputFile;	
		else return self::$inputFile;
	}
	/*Funkcia na overenie output suboru*/
	public function F_output_file()
	{
		if((self::$outputFile) == null)
				return self::std_outputFile;
		else return self::$outputFile;
	}
	/*Funkcia na overenie -h=subst*/
	public function F_substring()
	{
		if((self::$subst) == null)
			return self::constant_substring;
		else return self::$subst;
	}
	/*Funkcia na overenie --start=n*/
	public function F_start()
	{
		if((self::$start) == null)
				return self::constant_start;
		else return self::$start;
	}
	/*Funkcia na overenie --array-name=array-element*/
	public function F_array()
	{
		if((self::$array) == null)
				return self::constant_array;
		else return self::$array;
	}
	/*Funkcia na overenie --item-name=item-element*/
	public function F_item()
	{
		if((self::$item) == null)
				return self::constant_item;
		else return self::$item;
	}	
	/*Funkcia na spracovanie vstupnych argumentov*/
	public function arguments(array $argv)
	{
		unset($argv[0]); //bere argumenty az od argv[1] co mi ulahcilo spracovanie
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
						exit(1);	
				else self::$help = true;
			}
			//substr vrati vybranu cast retazca
			elseif(substr($vstup, 0, 8) === '--input=')
			{
					if((self::$inputFile)  !== null)	
						 	exit(1);
					$pom1 = substr($vstup, 8);	 
					self::$inputFile = $pom1;
			}
			elseif(substr($vstup, 0, 9) === '--output=')
			{
					if((self::$outputFile)  !== null)	
						 	exit(1);
					$pom2 = substr($vstup, 9);	 
					self::$outputFile = $pom2;
			}
			elseif($vstup === '-n')
			{
					if(self::$header)	
						 	exit(1);
					self::$header = true;
			}
			elseif(substr($vstup, 0, 3) === '-h=')
			{
					if((self::$subst)  !== null)	
						 	exit(1);
					$pom3 = substr($vstup, 3);		 
					self::$subst = $pom3;
			}
			elseif(substr($vstup, 0, 3) === '-r=')
			{
					if((self::$root_element)  !== null)	
						 	exit(1);
					$pom4 = substr($vstup, 3);	 
					self::$root_element = $pom4;
			}
			elseif(substr($vstup, 0, 13) === '--array-name=')
			{
					if((self::$array)  !== null)	
						 	exit(1);
					$pom5 = substr($vstup, 13);	
					$substring = substr($pom5,0,1);	
					$testChars1 ='/^[a-zA-Z_]{1,}$/';
					//porovnanie ci prve znaky nazvu elementu su povolene
					if(!preg_match($testChars1, $substring, $pom))
					{
						exit(50);
					}
					$testChars ='/^[a-zA-Z0-9_-ľščťžýáíéóú]{1,}$/';
					if(!preg_match($testChars, $pom5, $pom))
					{
						exit(50);
					}

					self::$array = $pom5;
			}
			elseif(substr($vstup, 0, 12) === '--item-name=')
			{
					if((self::$item)  !== null)	
						 	exit(1);
					$pom6 = substr($vstup, 12);	

					$substring = substr($pom6,0,1);	
					$testChars1 ='/^[a-zA-Z_]{1,}$/';
					if(!preg_match($testChars1, $substring, $pom))
					{
						exit(50);
					}

					$testChars2 ='/^[a-zA-Z0-9_-ľščťžýáíéóú]{1,}$/';
					if(!preg_match($testChars2, $pom6, $pom))
					{
						exit(50);
					}
					self::$item = $pom6;
			}
			elseif($vstup === '-s')
			{
					if(self::$string)	
						 	exit(1);
					self::$string = true;
			}
			elseif($vstup === '-i')
			{
					if(self::$number)	
						 	exit(1);
					self::$number = true;
			}
			elseif($vstup === '-l')
			{
					if(self::$boolean)	
						 	exit(1);
					self::$boolean = true;
			}
			elseif($vstup === '-c')
			{
					if(self::$convert)	
						 	exit(1);
					self::$convert = true;
			}
			elseif($vstup === '-a' || $vstup === '--array-size')
			{
					if(self::$array_size)	
						 	exit(1);
					self::$array_size = true;
			}
			elseif($vstup === '-t' || $vstup === '--index-items')
			{
					if(self::$index_items)	
						 	exit(1);
					self::$index_items = true;
			}
			elseif(substr($vstup, 0, 8) === '--start=')
			{
					if((self::$start)  !== null)
							exit(1);
					$pom7 = substr($vstup, 8);		
					self::$start = $pom7;

					if(!is_numeric(self::$start) OR self::$start < 0)
						exit(1);
					//mozno bude treba este nejako osetrit pretoze by
					//to malo byt vzdy s parametrom --index-items
			}
			else			
				exit(1);
		}
		if(((self::$start)!== null) AND !self::$index_items)
			 	exit(1);

			$pom = new xml(self::F_array(), 0);
			$pom->subst;
			$pom->zapisDoXML();
			$pom = new xml(self::F_item(), 0);
			$pom->subst;
			$pom->zapisDoXML();

			if(self::$root_element)
			{
				$testChars = '/^(-|\'|\*|&|@|<|[0-9]+)/';
				if(preg_match($testChars, self::$root_element, $out))
				{
						exit(50);
				}
				$pom = new xml(self::$root_element, 0);
				$pom->subst;
				$pom->zapisDoXML();
			}
	}		
}
/*..................ARG..................*/

/*..................JSON.................*/
/*Trieda na spracovanie JSON suboru*/
class json
{	
	//funkcia pri ktorej vyuzijeme preg_match
	//preg_match vie odtestovat ci retazec vyhovuje regularnemu vyrazu
	public function JSONdecode($retazec)
	{
		if(!preg_match('//u', $retazec))
				exit(4);
		$pom2 = json_decode($retazec);

		if(json_last_error())
			exit(4);
		return $pom2;	
	}
}
/*..................JSON..................*/

/*..................XML..................*/
/*Trieda XML, ktora ma rozne pomocne funkcie na spracovanie a nasledne vypis XML suboru*/
class xml
{
	// vytvorene pomocne pole
	private $atributy = array();
	private $data;
	private $meno;
	public $subst = true;
	private $pole;
	//PHP 5 allows developers to declare constructor methods for classes
	public function __construct($meno, $data)
	{
		$this->meno = $meno;

		if(is_object($data))
		{
				$this->data = array();
				foreach($data as $key => $val)
				{
						$pom4 = new xml($key, $val);
						$this->data[] = $pom4;
				}
		}
		elseif(is_array($data))
		{
			$pom1 = arg::F_start();
			$pom3 = count($data)-1;
			$pom2 = strlen($pom1+ $pom3);
			$this->pole = true;

			foreach($data as $key => $val)
			{
				$this->data[] = new xml(arg::F_item(), $val);
				if(arg::$index_items)
						$this->data[count($this->data) - 1]->toString($pom1, 'index');
					$pom1++;
			}
		}
		else
				$this->data = $data;
	}
	private function toString($pom1, $index)
	{
			$this->atributy[$index] = $pom1;
	}

	private function F_atributy()
	{ // pomocna funkcia ktora formatuje retazec s atributmi
			$pom2 = '';
			if(count($this->atributy) != 0)
			{
				foreach($this->atributy as $key => $val)
				{
					$pom1 = ' '.$key.'="'.$val.'"';
					$pom2 .= $pom1;
				}
				return $pom2;
			}
			else
				return $pom2;
	}
	/*Funkcia pre specialne charaktery*/
	private function F_escape($meno)
	{
		if($this->subst)
		{
			$chars = '/(\?|^[0-9]|\(|<|{|[|\)|]|}|>|;|,|%|\*|"|!|#|&|=|\+|\`|~|@|\/|^-)/';
			$testChars = '/^(-|\'|\*|@|<|[0-9]+)/';
			$meno = preg_replace($chars, arg::F_substring(), $meno);
			//funkcia kde preg_match vie odtestovat ci retazec vyhovuje regularnemu vyrazu
			if(preg_match($testChars, $meno, $pom))
			{
					//ak je invalid, vrati code 51
					exit(51);
			}
			else
				//ak vyhovuje, tak ho vracia
				return $meno;
		}
	}
	/*Funkcia vracia null, zaokruhlenu hodnotu, true, false...*/
	private function F_value($value)
	{ 
		if(is_float($value))
				//zaokruhli cislo 
				return round($value);
		elseif(($value) === null)
				return 'null';			
		elseif(is_bool($value))
		{
			if( $value == 'true')
				return 'true';
			else
				return 'false';
		}
		else
		{//htmlspecialchars klíčová funkce, která převede speciální znaky v textu na HTML entity
			$pom1 = htmlspecialchars($value);
			$pom2 = str_replace('"','&lt;&gt;&amp;&quot;&apos;',$value);
			if(arg::$convert == $pom1)
				return $pom1;
			else
				return $pom2;
		}
	}
	/*Funkcia zapise vysledok do xml suboru*/		
	public function zapisDoXML()
	{
	  //pouziva tu viacere pomocne funkie na spracovanie, ako F_escape, atd...
		$pom = '';
		if(is_array($this->data))
		{
			$pom = $this->isArray($pom);
		}
		elseif(!is_array($this->data))
		{
			if((is_bool($this->data) OR is_null($this->data)) AND arg::$boolean)
				$pom = $this->boolnull($pom);
			elseif((is_string($this->data)  AND arg::$string) OR ((is_int($this->data) OR is_float($this->data)) AND arg::$number))
				$pom = $this->stringIntFloat($pom);	
			else
				$pom = $this->value($pom);	
		}
		return $pom;
	}
	/*Funkcia pre konkretny vypis*/
	public function isArray($pom)
	{
			if(($this->meno) !== null)
					$pom .= '<'.$this->F_escape($this->meno).$this->F_atributy().'>'."\n";
			if($this->pole) // pri zadani -a alebo --array-size doplni size u pole
					$pom .= '<'.$this->F_escape(arg::F_array()).(arg::$array_size ? ' size="'.count($this->data).'"' : '').'>'."\n";
			foreach($this->data as $dat)
		  			$pom .= $dat->zapisDoXML();
			if($this->pole)
					$pom .= '</'.$this->F_escape(arg::F_array()).'>'."\n";
			if(!is_null($this->meno))
					$pom .= '</'.$this->F_escape($this->meno).'>'."\n";
			return $pom;
	}
	/*Funkcia pre konkretny vypis*/
	public function boolnull($pom)
	{
			$pom .= '<'.$this->F_escape($this->meno).$this->F_atributy().'>'."\n";
			$pom .= '<'.$this->F_value($this->data).'/>'."\n";
			$pom .= '</'.$this->F_escape($this->meno).'>'."\n";
			return $pom;
	}
	/*Funkcia pre konkretny vypis*/
	public function stringIntFloat($pom)
	{
			$pom .= '<'.$this->F_escape($this->meno).$this->F_atributy().'>'.$this->F_value($this->data).'</'.$this->F_escape($this->meno).'>'."\n";
			return $pom;
	}
	/*Funkcia pre konkretny vypis*/
	public function value($pom)
	{
			$pom .= '<'.$this->F_escape($this->meno).$this->F_atributy().' value="'.$this->F_value($this->data).'"/>'."\n";
			return $pom;
	}
}
/*..................XML..................*/

/*show help zobrazi napovedu pri zadani --help*/
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
/*****************END******************/
?>
