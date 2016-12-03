<?php
/**
#JSN:xnechu01
 *
 * @author	Stanislav Nechutný - xnechu01
 * @created	2015-02-21
 * @license	GNU GPLv2
 */
// Run Forrest, run!
try
{
	arguments::load($argv);
	if(arguments::$help)
	{
		display_help();
		die(0);
	}
	$data = json::decode(file::read(arguments::getInputFile()), arguments::$error);
	if(arguments::$flat)
	{ // Flat input
		$data = new flat($data);
		$data = $data->output();
	}
	$prepand = '';
	if(arguments::$header)
	{
		$prepand = '<?xml version="1.0" encoding="UTF-8"?>'."\n";
	}
	$x = new XMLElement(arguments::$root, $data);
	file::write(arguments::getOutputFile(), $prepand.$x->output());
}
catch(exitException $e)
{
	echo $e;
	die($e->code);
}
class XMLElement
{
	/* Element Data */
	private $data = NULL;
	/* Element name */
	private $name = NULL;
	/* Element attributes - key name, value is value */
	private $attributes = array();
	/* Indent level for pretty print */
	private $indent = 0;
	/* Is array? */
	private $isArray = FALSE;
	/* Disable element name substitution, TRUE for enabling */
	private $subst = TRUE;
	/**
	 * Construct new XML element class
	 *
	 * @param	string	$name	Element name
	 * @param	mixed	$data	Value of element
	 * @param	int	$indent	Indention for beautiful print
	 *
	 * @return	XMLElement
	 */
	public function __construct($name, $data, $indent = 0)
	{
		// Better indention in unnamed elements
		if(is_null($name))
		{
			$indent -= 1;
		}
		$this->indent = $indent;
		$this->name = $name;
		// Is array, so handle all elements and create tree
		if(is_array($data))
		{
			$this->isArray = true;
			$i = arguments::getStartIndex();
			$pad = strlen($i + count($data)-1);
			foreach($data as $key => $val)
			{
				$this->data[] = new XMLElement(arguments::getItemName(), $val, $indent+2);
				if(arguments::$index)
				{
					if(arguments::$padding)
					{
						$i = str_pad($i, $pad, '0', STR_PAD_LEFT);
					}
					$this->data[ count($this->data)-1 ]->setAttribute('index', $i);
				}
				$i++;
			}
		}
		// Is object, so handle items
		elseif(is_object($data))
		{
			$this->data = array();
			foreach($data as $key => $val)
			{
				$this->data[] = new XMLElement($key, $val, $indent+1);
			}
		}
		// Key - value
		else
		{
			$this->data = $data;
		}
	}
	/**
	 * Enable/Disable XML element name substitution
	 *
	 * @param	$value	bool	True for enable, False disable
	 */
	public function setSubstitution($value)
	{
		$this->subst = $value;
	}
	/**
	 * Convert XML tree to string and return it
	 *
	 * @return	string	XML document
	 */
	public function output()
	{
		$result = '';
		if(is_array($this->data))
		{ // Is array, or object, so create root element and items
			if(!is_null($this->name))
			{ // Create root element, only if its name is not null
				$result .= $this->indent().'<'.$this->escapeTagName($this->name).$this->getAttributes().'>'."\n";
			}
			if($this->isArray)
			{ // Create array element if is array
				$result .= $this->indent($this->indent+1).'<'.$this->escapeTagName(arguments::getArrayName()).(arguments::$size ? ' size="'.count($this->data).'"' : '').'>'."\n";
			}
			// Convert all childs to string
			foreach($this->data as $dat)
			{
				$result .= $dat->output();
			}
			if($this->isArray)
			{ // Close array element if is array
				$result .= $this->indent($this->indent+1).'</'.$this->escapeTagName(arguments::getArrayName()).'>'."\n";
			}
			if(!is_null($this->name))
			{ // Close root element, only if its name is not null
				$result .= $this->indent().'</'.$this->escapeTagName($this->name).'>'."\n";
			}
		}
		else
		{
			// Only key-value so do it
			if( (is_null($this->data) OR is_bool($this->data)) AND arguments::$literal)
			{ // Literal
				$result .= $this->indent().'<'.$this->escapeTagName($this->name).$this->getAttributes().'>'."\n";
				$result .= $this->indent($this->indent+1).'<'.$this->escapeValue($this->data).' />'."\n";
				$result .= $this->indent().'</'.$this->escapeTagName($this->name).'>'."\n";
			}
			elseif( ((is_float($this->data) OR is_int($this->data) ) AND arguments::$number) OR (is_string($this->data)  AND arguments::$string) )
			{ // Number or string
				$result .= $this->indent().'<'.$this->escapeTagName($this->name).$this->getAttributes().'>'.$this->escapeValue($this->data).'</'.$this->escapeTagName($this->name).'>'."\n";
			}
			else
			{
				$result .= $this->indent().'<'.$this->escapeTagName($this->name).$this->getAttributes().' value="'.$this->escapeValue($this->data).'" />'."\n";
			}
		}
		return $result;
	}
	/**
	 * Set element attribute
	 *
	 * @param	string	$name	Attribute name
	 * @param	string	$value	Attribute name
	 */
	public function setAttribute($name, $value)
	{
		$this->attributes[$name] = $value;
	}
	/**
	 * Create string with attribues
	 *
	 * Formate string with attributes for usage in generated XML
	 *
	 * @return	string	Attributes
	 */
	public function getAttributes()
	{
		if(count($this->attributes) == 0)
		{
			return '';
		}
		else
		{
			$attr = '';
			foreach($this->attributes as $key => $val)
			{
				$attr .= ' '.$key.'="'.$val.'"';
			}
			return $attr;
		}
	}
	/**
	 * Return correct indention via tabulators
	 *
	 * @param	int|NULL	$num	Indention level
	 *
	 * @return	string	Indention
	 */
	private function indent($num = NULL)
	{
		if(is_null($num))
		{
			$num = $this->indent;
		}
		return str_pad('', $num, "\t");
	}
	/**
	 * Escape tag name
	 *
	 * Replace all other characters, than latters, numbers,
	 * minus or underscore with placeholder
	 *
	 * @param	string	$name	Tag name
	 *
	 * @return	string	Escaped tag name
	 */
	private function escapeTagName($name)
	{
		if($this->subst)
		{ // escape element name
			$name = preg_replace('/(\?|<|>|\(|\)|{|}|[|]|,|;|\*|%|!|"|&|#|\+|=|~|\`|@|\/|\s|^-|^[0-9])/', arguments::getSubst(), $name);
		}
		if(preg_match('/^(-|[0-9]+)/', $name, $out))
		{ // Check name after escape
			throw new exitException('Element name "'.$name.'" is invalid.', 51);
		}
		return $name;
	}
	/**
	 * Escape tag value
	 *
	 * Escape tag value for special characters eg. " and formate
	 * it to correct value (nulls, booleans etc)
	 *
	 * @param	mixed	$value Value to be escaped
	 *
	 * @return	string	Escaped valued for usage in XML
	 */
	private function escapeValue($value)
	{
		if(is_null($value))
		{
			return 'null';
		}
		else if(is_bool($value))
		{
			return $value ? 'true' : 'false';
		}
		else if(is_float($value))
		{
			return floor($value);
		}
		else
		{
			return arguments::$convert ? htmlspecialchars($value) : str_replace('"','&quot;',$value);
		}
	}
}
class arguments
{
	static
		/* Input file */
		$inFile		= NULL,
		/* Output file */
		$outFile	= NULL,
		/* Substitution character */
		$subst		= NULL,
		/* Root element */
		$root		= NULL,
		/* Generate xml header */
		$header		= TRUE,
		/* Name of array element */
		$n_array	= NULL,
		/* Name of item element */
		$n_item		= NULL,
		/* Convert string values */
		$string		= FALSE,
		/* Convert number values */
		$number		= FALSE,
		/* Convert literal values (bool, null) */
		$literal	= FALSE,
		/* Escape XML */
		$convert	= FALSE,
		/* Add size to arrays */
		$size		= FALSE,
		/* Add indexes to array items */
		$index		= FALSE,
		/* Start array item's index with specific number */
		$start		= NULL,
		/* Padding numbers with zero */
		$padding	= FALSE,
		/* Flat array structure */
		$flat		= FALSE,
		/* Recorvery JSON errors */
		$error		= FALSE,
		/* Show only help */
		$help		= FALSE;
	/* Default substitution */
	const subst	= '-';
	/* Default array element name */
	const n_array	= 'array';
	/* Default item name */
	const n_item	= 'item';
	/* Array index start */
	const start	= 1;
	/* Default input file */
	const inFile	= 'php://stdin';
	/* Default output file */
	const outFile	= 'php://stdout';
	/**
	 * Load cli arguments and set properties
	 *
	 * @param	array	$argv	CLI arguments ($argv[0] included)
	 */
	public static function load(array $argv)
	{
		// Delete script name from arguments
		unset($argv[0]);
		foreach($argv as $arg)
		{
			// Standardize arguments with '--' prefix to '-'
			if(substr($arg, 0, 2) === '--')
			{
				$arg = substr($arg, 1);
			}
			if($arg === '-help')
			{
				if(self::$help)
				{
					throw new exitException('Help argument already specified.', 1);
				}
				self::$help = TRUE;
			}
			elseif(substr($arg, 0, 7) === '-input=')
			{
				if(!is_null(self::$inFile))
				{
					throw new exitException('Input file already specified.', 1);
				}
				self::$inFile = substr($arg, 7);
			}
			elseif(substr($arg, 0, 8) === '-output=')
			{
				if(!is_null(self::$outFile))
				{
					throw new exitException('Output file already specified.', 1);
				}
				self::$outFile = substr($arg, 8);
			}
			elseif(substr($arg, 0, 3) === '-h=')
			{
				if(!is_null(self::$subst))
				{
					throw new exitException('Substitution already specified.', 1);
				}
				self::$subst = substr($arg, 3);
			}
			elseif(substr($arg, 0, 3) === '-r=')
			{
				if(!is_null(self::$root))
				{
					throw new exitException('Root element already specified.', 1);
				}
				self::$root = substr($arg, 3);
			}
			elseif(substr($arg, 0, 12) === '-array-name=')
			{
				if(!is_null(self::$n_array))
				{
					throw new exitException('Array element already specified.', 1);
				}
				self::$n_array = substr($arg, 12);
			}
			elseif(substr($arg, 0, 11) === '-item-name=')
			{
				if(!is_null(self::$n_item))
				{
					throw new exitException('Item element already specified.', 1);
				}
				self::$n_item = substr($arg, 11);
			}
			elseif($arg === '-s')
			{
				if(self::$string)
				{
					throw new exitException('String conversion already specified.', 1);
				}
				self::$string = TRUE;
			}
			elseif($arg === '-n')
			{
				if(!self::$header)
				{
					throw new exitException('No generating xml header already specified.', 1);
				}
				self::$header = FALSE;
			}
			elseif($arg === '-i')
			{
				if(self::$number)
				{
					throw new exitException('Number conversion already specified.', 1);
				}
				self::$number = TRUE;
			}
			elseif($arg === '-l')
			{
				if(self::$literal)
				{
					throw new exitException('Literal conversion already specified.', 1);
				}
				self::$literal = TRUE;
			}
			elseif($arg === '-c')
			{
				if(self::$convert)
				{
					throw new exitException('XML conversion already specified.', 1);
				}
				self::$convert = TRUE;
			}
			elseif($arg === '-a' || $arg === '-array-size')
			{
				if(self::$size)
				{
					throw new exitException('Adding array size already specified.', 1);
				}
				self::$size = TRUE;
			}
			elseif($arg === '-t' || $arg === '-index-items')
			{
				if(self::$index)
				{
					throw new exitException('Adding index to array elements already specified.', 1);
				}
				self::$index = TRUE;
			}
			elseif(substr($arg,0,7) === '-start=')
			{
				if(!is_null(self::$start))
				{
					throw new exitException('Index start already specified.', 1);
				}
				self::$start = substr($arg, 7);
				if((int)self::$start != self::$start OR self::$start < 0)
				{
					throw new exitException('Index start have incorrect value.', 1);
				}
			}
			elseif($arg === '-padding')
			{
				if(self::$padding)
				{
					throw new exitException('Padding already specified.', 1);
				}
				self::$padding = TRUE;
			}
			elseif($arg === '-flattening')
			{
				if(self::$flat)
				{
					throw new exitException('Flattening already specified.', 1);
				}
				self::$flat = TRUE;
			}
			elseif($arg === '-error-recovery')
			{
				if(self::$error)
				{
					throw new exitException('Error recovery already specified.', 1);
				}
				self::$error = TRUE;
			}
			else
			{
				throw new exitException('Unkown argument "'.$arg.'".', 1);
			}
		}
		if(count($argv) > 1 AND self::$help)
		{
			throw new exitException('Help argument mixed with another option.', 1);
		}
		if(!is_null(self::$start) AND !self::$index)
		{
			throw new exitException('Specified index start, but not set to add indexes to array items.', 1);
		}
		// Check array, root and item element name (with disabled(!) substitution
		try
		{
			if(self::$root)
			{
				$x = new XMLElement(self::$root, 0);
				$x->setSubstitution(FALSE);
				$x->output();
			}
			$x = new XMLElement(self::getArrayName(), 0);
			$x->setSubstitution(FALSE);
			$x->output();
			$x = new XMLElement(self::getItemName(), 0);
			$x->setSubstitution(FALSE);
			$x->output();
		}
		catch(exitException $e)
		{
			throw new exitException('Incorrect element name based on argument.', 50);
		}
	}
	/**
	 * Get name of element for array
	 *
	 * @return	string	Element name
	 */
	public static function getArrayName()
	{
		if(!is_null(self::$n_array))
		{
			return self::$n_array;
		}
		else
		{
			return self::n_array;
		}
	}
	/**
	 * Get name of element for array item
	 *
	 * @return	string	Element name
	 */
	public static function getItemName()
	{
		if(!is_null(self::$n_item))
		{
			return self::$n_item;
		}
		else
		{
			return self::n_item;
		}
	}
	/**
	 * Get input file path
	 *
	 * @return	string	Path
	 */
	public static function getInputFile()
	{
		if(!is_null(self::$inFile))
		{
			return self::$inFile;
		}
		else
		{
			return self::inFile;
		}
	}
	/**
	 * Get output file path
	 *
	 * @return	string	Path
	 */
	public static function getOutputFile()
	{
		if(!is_null(self::$outFile))
		{
			return self::$outFile;
		}
		else
		{
			return self::outFile;
		}
	}
	/**
	 * Get array start index value
	 *
	 * @return	int	Index
	 */
	public static function getStartIndex()
	{
		if(!is_null(self::$start))
		{
			return self::$start;
		}
		else
		{
			return self::start;
		}
	}
	/**
	 * Get substitution string
	 *
	 * @return	string	String
	 */
	public static function getSubst()
	{
		if(!is_null(self::$subst))
		{
			return self::$subst;
		}
		else
		{
			return self::subst;
		}
	}
}
class json
{
	/**
	 * Decode JSON string to stdClass object
	 *
	 * Can try fix damaged JSON
	 *
	 * @param	string	$str	JSON
	 * @param	bool	$fix	Try fix JSON
	 *
	 * @return	mixed	Decoded JSON
	 */
	public static function decode($str, $fix = FALSE)
	{
		$str = trim($str);
		if(!preg_match('##u', $str))
		{ // Fix PHP < 5.3.3
			exitException('Invalid UTF-8 in JSON', 4);
		}
		$result = json_decode($str);
		if(is_null($result) AND $str !== 'null')
		{
			if(!$fix)
			{
				throw new exitException('Readed invalid JSON - '.json_last_error(), 4);
			}
			else
			{
				// Fix indexes without quotes
				$str = preg_replace('/([{[,]\s*)(\b.*\b)(\s*:.*)/Um', '$1"$2"$3', $str);
				// Fix missing " at end of line
				$str = preg_replace('/(.*)"((.*)[^"])(,)$/m', '$1"$2"$3$4', $str);
				// Fix missing " before comma
				$str = preg_replace('/(.*[^"\]}])\s*(,)\s*($)/Um', '$1"'."\n".'$2$3', $str);
				// Comma before ], or }
				$str = preg_replace('/(.*)(,)\s+([}\]])/Um', '$1$3', $str);
				// Missing comma between " and "
				$str = preg_replace('/(.*")\s*(".*)/Um', '$1,$2', $str);
				// Missing " before semicolon
				$str = preg_replace('/(.*[{,]\s*)("[^":\s]*)(\s*:\s*.*)$/m', '$1$2"$3', $str);
				// Add missing ] or } at end of JSON
				if($str[0] === '[' AND substr($str, -1) !== ']')
				{
					$str .= ']';
				}
				if($str[0] === '{' AND substr($str, -1) !== '}')
				{
					$str .= '}';
				}
				return json::decode($str, FALSE);
			}
		}
		return $result;
	}
}
/**
 * Help class for throwing error exceptions with exit code.
 */
class exitException extends Exception
{
	public $code, $message;
	public function __construct($message, $code)
	{
		$this->message = $message;
		$this->code = $code;
	}
	public function __toString()
	{
		return "Error: ".$this->message." (Code: ".$this->code.")\n";
	}
}
class flat
{
	/**
	 * Input data to be converted
	 */
	private $data;
	private $result = NULL;
	/**
	 * Construct object
	 *
	 * @param	mixed	$data	Data to be flatened
	 */
	public function __construct($data)
	{
		$this->data = $data;
	}
	/**
	 * Get flattened result
	 *
	 * @return	stdClass
	 */
	public function output()
	{
		if(is_null($this->result))
		{
			$this->result = new stdClass();
			$this->recursiveWalk(NULL, $this->data);
			foreach($this->result as $key => $val)
			{
				if(is_array($val))
				{
					usort($this->result[$key], array($this, 'cmp'));
				}
			}
		}
		return $this->result;
	}
	/**
	 * Compare two values
	 *
	 * @param	$a	mixed	First value
	 * @param	$b	mixed	Second value
	 *
	 * @return	int	Compared result - -1, 0, 1
	 */
	private function cmp($a, $b)
	{
		if(is_null($a))
		{ // Null is lowest
			if(is_null($b))
			{
				return 0;
			}
			else
			{
				return -1;
			}
		}
		if(is_bool($a) && !$a)
		{ // False second lowest
			if(is_bool($b) && !$b)
			{
				return 0;
			}
			elseif(is_null($b))
			{
				return 1;
			}
			else
			{
				return -1;
			}
		}
		if(is_bool($a) && $a)
		{ // True third lowest
			if(is_bool($b) && $b)
			{
				return 0;
			}
			elseif(is_bool($b) || is_null($b))
			{
				return 1;
			}
			else
			{
				return -1;
			}
		}
		// Same?
		if($a == $b)
		{
			return 0;
		}
		// Compare rest
		return $a > $b ? 1 : -1;
	}
	/**
	 * Flat all members in code
	 *
	 * @param	$key	mixed	Value key
	 * @param	$dat	mixed	Value
	 */
	private function recursiveWalk($key, $data)
	{
		if(is_array($data))
		{
			if(count($data) > 0)
			{ // find lowest value in array
				usort($data, array($this, 'cmp'));
				reset($data);
				$min = current($data);
				if(!is_array($min) AND !is_object($min))
				{ // and put it to result if is not array nor object
					$this->recursiveWalk($key, $min);
				}
			}
		}
		elseif(is_object($data))
		{ // Object, so walk through it
			foreach($data as $k => $v)
			{
				$this->recursiveWalk($k ,$v);
			}
		}
		elseif(!is_null($key))
		{ // is key - value and key is not null
			if(isset($this->result->{$key}))
			{
				if(is_array($this->result->{$key}))
				{ // already is array, so just add
					$this->result->{$key}[] = $data;
				}
				else
				{ // is key-value, so convert to key - array
					$this->result->{$key} = array(
						$this->result->{$key},
						$data,
					);
				}
			}
			else
			{ // create key - value
				$this->result->{$key} = $data;
			}
		}
	}
}
class file
{
	/**
	 * Read content from file
	 *
	 * @param	$fileName	string	Path to file
	 *
	 * @return	string	Content of file
	 */
	public static function read($fileName)
	{
		if(!is_readable($fileName) AND $fileName != arguments::inFile)
		{
			throw new exitException("Can't open file for read.", 2);
		}
		$x = @file_get_contents($fileName);
		if($x == FALSE)
		{
			throw new exitException("Can't open file for read.", 2);
		}
		return $x;
	}
	/**
	 * Write content to file
	 *
	 * @param	$fileName	string	Path to file
	 * @param	$content	string	Content to write
	 */
	public static function write($fileName, $content)
	{
		$x = @file_put_contents($fileName, $content);
		if($x == FALSE)
		{
			throw new exitException("Can't open file for write.", 2);
		}
	}
}
/**
 * Print program usage
 */
function display_help()
{
	echo "
--help
	viz společné zadání všech úloh
-input=filename
	zadaný vstupní JSON soubor v kódování UTF-8
--output=filename
	textový výstupní XML soubor v kódování UTF-8 s obsahem převedeným ze vstupního souboru
-h=subst
	ve jméně elementu odvozeném z dvojice jméno-hodnota nahraďte každý nepovolený znak ve jméně XML značky řetězcem subst. Implicitně (i při nezadaném parametru -h) uvažujte nahrazování znakem pomlčka (-). Vznikne-li po nahrazení invalidní jméno XML elementu, skončete s chybou a návratovým kódem 51.
-n
	negenerovat XML hlavičku na výstup skriptu (vhodné například v případě kombinovánívíce výsledků)
-r=root-element
	jméno párového kořenového elementu obalující výsledek. Pokud nebude zadán, tak se výsledek neobaluje kořenovým elementem, ač to potencionálně porušuje validitu XML (skript neskončí s chybou). Zadání řetězce root-element vedoucího na nevalidní XML značku ukončí skript s chybou a návratovým kódem 50 (nevalidní znaky nenahrazujte).
--array-name=array-element
	tento parametr umožní přejmenovat element obalující pole z implicitní hodnoty array na array-element. Zadání řetězce array-element vedoucího na nevalidní XML značku ukončí skript s chybou a návratovým kódem 50 (nevalidní znaky nenahrazujte).
--item-name=item-element
	analogicky, tímto parametrem lze změnit jméno elementu pro prvky pole (implicitní hodnota je item). Zadání řetězce item-element vedoucího na nevalidní XML značku ukončí skript s chybou a návratovým kódem 50 (nevalidní znaky nenahrazujte).
-s
	hodnoty (v dvojici i v poli) typu string budou transformovány na textové elementy místo atributů.
-i
	hodnoty (v dvojici i v poli) typu number budou transformovány na textové elementy místo atributů.
-l
	hodnoty literálů (true, false, null) budou transformovány na elementy <true/>, <false/> a <null/> místo na atributy.
-c
	tento přepínač oproti implicitnímu chování aktivuje překlad problematických znaků. Pro XML problematické znaky s UTF-8 kódem menším jak 128 ve vstupních řetězcových hodnotách (ve dvojicích i polích) konvertujte na odpovídající zápisy v XML pomocí metaznaku & (např. &amp;, &lt;, &gt;atd.). Ostatní problematické znaky konvertovat nemusíte.
-a, --array-size
	u pole bude doplněn atribut size s uvedením počtu prvků v tomto poli.
-t, --index-items
	ke každému prvku pole bude přidán atribut index s určením indexu prvku v tomto poli (číslování začíná od 1, pokud není parametrem --start určeno jinak).
--start=n
	inicializace inkrementálního čitače pro indexaci prvků pole (nutno kombinovat s parametrem --index-items, jinak chyba 1) na zadané kladné celé číslo n včetně nuly (implicitně n = 1).
JPD (0,5 bodu): Podpora volby --padding, která u použitých čitačů (nutno kombinovat s parametrem --index-items, jinak chyba 1) provede doplnění takového počtu nul zleva, aby všechna čísla dané sekvence měla stejný počet číslic, který bude ale minimální možný.
FLA (1,0 bodu): Podpora volby --flattening, která provede zploštění vstupního hierarchického formátu následujícím způsobem. Na výstup budou převedeny pouze listové dvojice jméno-hodnota (kde hodnota je skalárního typu, tj. žádný další objekt). Pokud je hodnota dvojice pole, tak z něj bude vzata pouze lexikograficky nejmenší skalární hodnota (ne objekt ani další pole) a zbytek prvků bude zahozen. Pokud takové pole žádnou skalární hodnotu neobsahovalo, tak bude z výstupu zcela vypuštěno. Při přidávání dvojic jméno-hodnota na nejvyšší úroveň mohou nastat tři případy: (1) dvojice je dle jména nová, pak provedeme obyčejné přidání; (2) jméno dvojice již ve výsledku existuje a jeho hodnota je skalární, pak je hodnota výstupní dvojice nahrazena polem obsahujícím původní i novou hodnotu pro dané jméno; (3) jméno dvojice již ve výsledku existuje a jeho hodnota je již pole, pak je do tohoto pole přidána nová hodnota. Pokud jsou v takto vzniklém poli výhradně číselné hodnoty, dojde k jejich vzestupnému seřazení dle velikosti. Pokud je některá hodnota vzniklého pole nečíselná, provede se řazení všech hodnot lexikograficky vzestupně (skalárním hodnotám null, false a true přiřaďte pro účely řazení postupně nejmenší, druhou nejmenší a třetí nejmenší ordinální hodnotu), ale výsledné hodnoty budou ve výstupu v původním typu (tj. konverze na řetězce a speciální ordinální hodnoty se provede pouze za účelem seřazení a nikoli výstupu). V poli se mohou vyskytovat vícenásobné výskyty stejných hodnot.
JER (1,0 bodu): Při zadání volby --error-recovery je skript schopen se zotavit z různých chyb při načítání vstupního souboru v JSON formátu (včetně jejich kombinací). Například doplnění chybějící nebo ignorace přebývající čárky nebo dvojtečky, doplnění chybějících koncových závorek zanořených polí před koncem objektu, doplnění chybějících koncových závorek zanořených objektů před koncem pole, doplnění chybějícího ukončení závorek před koncem vstupního souboru, ignorace přebývajících závorek na konci vstupu, doplnění zapomenutých uvozovek u jednoslovných jmen a hodnot bez speciálních znaků atd.
";
}
