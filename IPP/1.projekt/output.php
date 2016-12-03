<?php 
public function output()
  {
    $result = '';
    if(is_array($this->data))
    {
      echo "array\n";
      if(is_null($this->name) === null)
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
      { // Close array element if is array
        $result .= $this->F_print($this->print+1).'</'.$this->escapeTagName(arg::F_array()).'>'."\n";
      }
      if(!is_null($this->name))
      { // Close root element, only if its name is not null
        $result .= $this->F_print().'</'.$this->escapeTagName($this->name).'>'."\n";
      }
    }
    else
    {
      // Only key-value so do it
      if( (is_null($this->data) OR is_bool($this->data)) AND arg::$number)
      { // Literal
        echo "hello\n";
        $result .= $this->F_print().'<'.$this->escapeTagName($this->name).$this->getAttributes().'>'."\n";
        $result .= $this->F_print($this->print+1).'<'.$this->escapeValue($this->data).' />'."\n";
        $result .= $this->F_print().'</'.$this->escapeTagName($this->name).'>'."\n";
      }
      /*elseif(is_string($this->data)) 
      {
        $result .= $this->F_print().'<'.$this->escapeTagName($this->meno).$this->getAttributes().'value="'.$this->escapeValue($this->data).'" />'."\n";
      }*/
      else
      { 
        $result .= $this->F_print().'<'.$this->escapeTagName($this->name).$this->getAttributes().'>'.$this->escapeValue($this->data).'</'.$this->escapeTagName($this->name).'>'."\n";
      }
    }
    return $result;
  }