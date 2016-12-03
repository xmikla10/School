#!/usr/bin/env python3

#MKA:xmikla10

import os
import sys
import re
import string

# funkce "main" reprezentuje hlavny beh programu
def main():
#done
	#spracuje vstupni argumenty
	arguments = processArguments()
	#spracuje vstupni soubor
	inputFile = processInput(arguments)
	#overi semanticke chyby, ... 
	semantic  = Semantic(inputFile)
	#overeni zda je automat dobre specifikony
	semantic.wellSpecAutomat()
	#ak je zadan -f alebo --find-non-finishing
	if ( arguments.mka_f == True ):
		semantic = semantic.pomFneukoncStav()
		if ( semantic ):
			saveTo = arguments.mka_output
			saveTo.write(str(semantic[0]))
			sys.exit()

	#ak je zadan -m alebo --minimize
	if ( arguments.mka_m == True ):
		semantic = semantic.minimalizacia()
	#pomocna promnenna reprezentujici nazev souboru	
	saveTo = arguments.mka_output
	#zapis do soubotu
	saveTo.write(str(semantic))

#********************************************************************************************#
class Arguments(object):
	 def __init__(self):
	 	self.mka_input = sys.stdin
	 	self.mka_output = sys.stdout
	 	self.mka_f = False
	 	self.mka_m = False
	 	self.mka_i = False
#********************************************************************************************#
#funkce pro spracovani vstupnich argumentov
def processArguments():
	arguments = Arguments()

	for x in sys.argv[1:]:
		if x == '--help':
			if len(sys.argv) !=  2:
				print('Zle zadan argument --help')
				print('Pre vypis napovedy zadajte len --help')
				sys.exit(1)
			print("""--help
						pre vypisanie napovedy
					--input=filename
						zadany vstupny subor filename s popisom dobre specifikovaneho konecneho automatu
					--output=filename
						zadany vstupny subor filename s popisom vystledneho ekvivalentniho konecneho
						automatu v predepsanem formatu vystupu
					-f, --find-non-finishing
						hleda neukoncujici stav zadaneho dobre specifikovaneho konecneho automatu.
						Parametr nelze kombinovat s parametrem -m(resp. --minimalize)
					-m, --minimize
						provede minimalizaci dobre specifikovaneho konecneho automatu.
						Parametr nelze kombinovat s parametrem -f(resp. --find-non-finishing)
					-i, --case-insensitive
						nebude bran ohled na velikost znaku pri porovnanani symbolu ci stavu, tj.
						ve vystupu potom budou vsechna velka pismena prevedena na mala""")
			sys.exit(0)

		if x == '-f' or x == '--find-non-finishing':
			if arguments.mka_f == True:  # pouzite viac ako raz
				sys.exit(1)
			elif arguments.mka_m == True:
				sys.exit(1)
			else:
				arguments.mka_f = True

		if x == '-m' or x == '--minimize':
			if arguments.mka_m == True:  # pouzite viac ako raz
				sys.exit(1)
			elif arguments.mka_f == True:
				sys.exit(1)
			else:
				arguments.mka_m = True

		if x == '-i' or x == '--case-insensitive':
			if arguments.mka_i == True: # pouzite viac ako raz
				sys.exit(1)
			else:
				arguments.mka_i = True

		else:
			#b-begin, m-middle, e-end
			b,m,e = x.partition('=') 	

			if b == '--input':
				if e != '':
					if open(e, encoding="utf-8", mode="r"):
						arguments.mka_input = open(e, encoding="utf-8", mode="r")
					else:
						print('Error : Cannot open file')
						sys.exit(1)

			if b == '--output':
				if e != '':
					if open(e, encoding='utf-8', mode="w"):
						arguments.mka_output = open(e, encoding="utf-8", mode="w")
					else:
						print('Error : Cannot write to file')
						sys.exit(1)

			if( b != '--input' and b != '--output' and x != '-m' and x != '--minimize' and x != '-f' and x != '--find-non-finishing' and x != '-i' and x != '--case-insensitive'):
				print( x + ' - neznamy argument, pre pomoc napis --help')
				sys.exit(1)
			
	return arguments
#********************************************************************************************#
#done
def toArray( first, z ,number, element):
	try:
		pom = first[first.index(element[z*3 + number].pom)]
		return pom
	except:
		print("Error Code 61")
		sys.exit(61)
#********************************************************************************************#
#done
class Semantic(object):
	#inicializuje objekty a kontroluje semanticke chyby
	def __init__(self, inputFile):
		self.pocStav = None
		self.konStav = []
		self.stav = []
		self.abeceda = []
		self.pravidlo = []
		self.pomPole = []
		#enumerate pouzivam preto lebo chcem prechadzat pole
		#a nechcem prist o pravidelne sa zvysujucu hodnotu
		for x, element in enumerate(inputFile):
			if ( x == 0 ):
				for stav in element:
					if ( stav.pom not in self.stav ):
						self.stav.insert(len(self.stav), Stav(stav.pom))

			if ( x == 1 ):
				for symbol in element:
					if ( symbol.pom not in self.abeceda ):
						self.abeceda.insert(len(self.abeceda), Symbol(symbol.pom))

			if ( x == 2 ):
				length = len(element) / 3
				for z in range( 0, int(length)):
					fromS = toArray( self.stav, z, 0, element)
					symbol = toArray( self.abeceda, z, 1, element)
					toS = toArray( self.stav, z, 2, element)

					pravidlo = Pravidlo(fromS, toS, symbol)

					if ( pravidlo not in self.pravidlo):
						self.pravidlo.insert( len(self.pravidlo), pravidlo)

			if ( x == 3 ):
				self.pocStav = toArray( self.stav, 0, 0, element)

			if ( x == 4 ):
				for konStavPom in element:
					konStav = self.stav[self.stav.index(konStavPom.pom)]
					if self.konStav == False: sys.exit(61)
					if (konStav not in self.konStav):
						self.konStav.insert(len(self.konStav), konStav)

		alfabet = len(self.abeceda)
		if( alfabet == 0): sys.exit(61)

#********************************************************************************************#
#done
# funkcia ktora overuje ci je automat dobre specifikovany
	def wellSpecAutomat(self):

		for stav in self.stav:
			if ( stav != self.pocStav ):
				pravidla = self.pomPravidlo(stav, None, 2, 2)
				length = len(pravidla)
				if (length == 0):
					pom = stav
					print("NonWellSpec")
					sys.exit(62)
				else:
					pom = False
			else:
				pom = False

		for stav in self.stav:
			for symbol in self.abeceda:
				pravidla = self.pomPravidlo(stav, symbol, 2, 1)
				length = len(pravidla)
				if ( (pravidla == False) or (length == 0) ):
					print("NonWellSpec")
					sys.exit(62)

		self.pomFneukoncStav()
		return True

#********************************************************************************************#
#done
# pomocna funkcia pre najdenie pravidla v konecnom automate
	def pomPravidlo(self, stav, symbol, pom1=1,pom2=1):
		if ( pom1 == 1 ): pom1 = 1
		else: pomArray = []

		for rule in self.pravidlo:
			if ( pom2 == 2 ): control = rule.toS
			else: control = rule.fromS

			if (control == stav):
				if(rule.symbol == symbol or symbol is None):
					if ( pom1 == 2 ):
						length = len(pomArray)
						pomArray.insert(length, rule)
					else: return rule

		if ( pom1 == 1 ): return False
		else: return pomArray	
#********************************************************************************************#
#este pozriet
#pomocna funkcia pre zistenie neukoncujuceho stavu
	def pomFneukoncStav(self):
		zasobnik = self.konStav[:]
		pomArray = []
		neukoncenyStav = []

		while zasobnik:
			current = zasobnik.pop()
			if current not in pomArray:
				length = len(pomArray)
				pomArray.insert( length, current)
			pravidla = self.pomPravidlo(current, None, 2, 2)

			for pravidlo in pravidla:
				if pravidlo.fromS not in pomArray:
					length = len(zasobnik)
					zasobnik.insert(length, pravidlo.fromS)

		for stav in self.stav:
			if stav not in pomArray:
				length = len(neukoncenyStav)
				neukoncenyStav.insert( length, stav)

		length = len(neukoncenyStav)

		if(neukoncenyStav):
			return neukoncenyStav

		if ( length > 1 ):
			print("NonWellSpec")
			sys.exit(62)
			
#********************************************************************************************#
	def __findGroup(self, groups, state):
		for group in groups:
			if state in group:
				return group

		return None
#********************************************************************************************#
# este overit ze ta funkcia findGroup tam nemusi byt
	def minimalizacia(self):
		groups = []
		tmpList = []
		newStates = []
		newSymbols = []
		newRules = []
		newFinal = []

		length = len(groups)
		groups.insert(length, StateGroup(self.konStav))

		for state in self.stav:
			if state not in self.konStav: tmpList.append(state)
		groups.append(StateGroup(tmpList))

		divided = True
		while divided:
			divided = False
			for group in groups:
				groupDict = dict()
				for symbol in self.abeceda:
					for state in group:
						rule = self.pomPravidlo(state, symbol, 1, 1)
						outState = rule.toS

						for pomGroup in groups:
							if( outState in pomGroup): outGroup = pomGroup

						if outGroup in groupDict: groupDict[outGroup].append(state)
						else: groupDict[outGroup] = [state]

					if len(groupDict) == 1: groupDict.clear()
					else:
						for splitGroup, states in groupDict.items():
							groups.append(StateGroup(states))

						groups.remove(group)
						divided = True
						break

				if divided: break
		for group in groups: group.stav.sort()
		for group in groups: newStates.append(elementElement(str(group), False))
		for symbol in self.abeceda: newSymbols.append(elementElement(symbol.name, True))

		for rule in self.pravidlo:
			for group in groups:
				if rule.fromS in group: fromS = group

			symbol = rule.symbol
			for group in groups:
				if rule.toS in group: toS = group

			length = len(newRules)
			newRules.insert(length, elementElement(str(fromS), False))
			length = len(newRules)
			newRules.insert(length, elementElement(symbol.name, True))
			length = len(newRules)
			newRules.insert(length, elementElement(str(toS), False))

		for group in groups:
			if self.pocStav in group:
				pom = group
				pom = str(pom)
		newStart = [ elementElement(pom, False) ]

		for group in groups:
			for finalState in self.konStav:
				if finalState in group:
					newFinal.append(elementElement(str(group), False))
					break

		return Semantic([newStates, newSymbols, newRules, newStart, newFinal])

#********************************************************************************************#
#done
	#pomocna funkce pro vypis stavu, abecedy, pravidel...
	def __str__(self):

		pom = "(\n{"
		char = ", "

		length = len(self.stav)
		if length == 1: pom = pomStr(self.stav, s, 0)
		elif length > 1:
			self.stav.sort()
			for stav in self.stav[0:-1]:
				string = str(stav)
				pom = pom + string + char
			pom = pomStr(self.stav, pom, -1)
		pom = pomStr2(pom, 1)

		length = len(self.abeceda)
		if length == 1: pom = pomStr(self.abeceda, pom, 0)
		elif length > 1:
			self.abeceda.sort()
			for symbol in self.abeceda[0:-1]:
				string = str(symbol)
				pom = pom + string + char
			pom = pomStr(self.abeceda, pom, -1)
		pom = pomStr2(pom, 5)

		length = len(self.pravidlo)
		if length == 1: pom = pomStr(self.pravidlo, pom, 0)
		elif length > 1:
			self.pravidlo.sort()
			for pravidlo in self.pravidlo[0:-1]:
				string = str(pravidlo)
				pom = pom + string + ",\n"
			pom = pomStr(self.pravidlo, pom, -1)
		pom = pomStr2(pom, 3)

		string = str(self.pocStav)
		if self.pocStav != None: pom+= string
		pom = pomStr2(pom, 4)

		length = len(self.konStav)
		if length == 1: pom = pomStr(self.konStav, pom, 0)
		elif length > 1:
			self.konStav.sort()
			for stav in self.konStav[0:-1]:
				string = str(stav)
				pom = pom + string + char
			pom = pomStr(self.konStav, pom, -1)
		pom= pomStr2(pom, 2)

		return pom
#********************************************************************************************#
#done
#pomocni funkce pro spracovani __str__          
def pomStr(x, pom, number):
	pom += str(x[number])
	return pom;
#********************************************************************************************#
#done
#pomocni funkce pro spracovani __str__  	            
def pomStr2(pom, number):
	if( number == 1):
		pom += "},\n{"
	elif( number == 2):
		pom += "}\n)"
	elif( number == 3):
		pom += "\n},\n"
	elif( number == 4):
		pom += ",\n{"
	else:
		pom += "},\n{\n"
	return pom
#********************************************************************************************#
class Struct:
	elements = [[], [], [] , [], []]
	pomSymbol = 0
	kaDef = False
	sucPrvok = False
	current = 0
	apostrof = 0

#********************************************************************************************#
#done
class elementElement(object):
	def __init__(self, pom, isSymbol): #premenovat isSymbol zistit co to je
		self.pom = pom
		self.isSymbol = isSymbol
#********************************************************************************************#
#done
def regularExpression(s1, regEx):
    for element in s1.elements:
        for elem in element:
            if (elem.isSymbol == False):
            	regEx = regEx
            	result = regEx.match(elem.pom)
            	if not result: return False
            	else: return True
			
#********************************************************************************************#
#done
def toEndOfList(s1, pom, trueOrFalse):
	if trueOrFalse == True:
		s1.elements[s1.current].insert(len(s1.elements[s1.current]), elementElement(pom, True))
	else:
		s1.elements[s1.current].insert(len(s1.elements[s1.current]), elementElement(pom, False))

#********************************************************************************************#		
def processInput(arguments):
    s1 = Struct()
    pom = ""
    charHash = '#'
    charRCbrace = '}'
    charLCbrace = '{'
    charRbrace = ')'
    charLbrace = '('
    charSlash = '\''
    charArrow = '->'
    charComma = ','

    for line in arguments.mka_input:
        for char in line:
            if (s1.pomSymbol == 1) :
                if (char != charSlash):
                    if (s1.apostrof != 1): pom = pom + char
                    else:
                    	toEndOfList(s1, pom, True)
                    	s1.pomSymbol = 0
                    	pom = ""
                elif(char == charSlash):
                	if (s1.apostrof == 1):
                		pom = pom + charSlash
                		s1.apostrof = 0
                	else: s1.apostrof = 1

    		#ak je komentar
            if (s1.pomSymbol == 0):
                if (str.isspace(char) and s1.sucPrvok == False): continue
                if char == charHash: break

                if (s1.kaDef == True):
                    if (s1.sucPrvok != False):
                        if (char == charRCbrace):
                            s1.sucPrvok = False
                            toEndOfList(s1, pom, False)
                            pom = ""
                        elif (char == charComma):
                            toEndOfList(s1, pom, False)
                            pom = ""
                            if s1.current == 3:
                            	s1.current = s1.current + 1
                            	s1.sucPrvok = False
                        elif (char == charSlash):
                            s1.apostrof = 0
                            s1.pomSymbol = 1
                            toEndOfList(s1, pom, False)
                            pom = ""
                        #dalsi znak
                        else: pom = pom + char
                    else:
                        if (char == charComma):
                            s1.current = s1.current + 1
                            if (s1.current == 3 ): s1.sucPrvok = True

                        elif (char == charRbrace):
                        	length = len(s1.elements)
                        	length = length - 1
                        	if (s1.current != length):
                        		print("Error Code 60")
                        		sys.exit(60)
                        	else: s1.kaDef = False

                        elif (char == charLCbrace): s1.sucPrvok = True
                        else:
                            print("Error Code 60")
                            sys.exit(60)
                else:
                    if ( char == charLbrace ): s1.kaDef = True
                    elif ( char != charLbrace ):
                        print("Error Code 60")
                        sys.exit(60)
               
        if (s1.pomSymbol == 1 ):
            pom = pom + '\n'

    length = len(s1.elements)
    length = length - 1
    if ( s1.current != length ):
    	print("Error Code 60")
    	sys.exit(60)

    stripStates(pom)

    checkRules(pom, arguments.mka_i)

    return s1.elements
#********************************************************************************************#
# Pomocna funkcia, ktora skontroluje pravidla a v pripade chyby vrati code: 60
def checkRules(pom, mka_i):
	s1 = Struct()
	charArrow = '->'

	pom = len(s1.elements[2])
	if (pom % 3 != 0):
		print("Error Code 60")
		sys.exit(60)

	for i, elem in enumerate(s1.elements[2]):
		if (i % 3 == 0 and elem.isSymbol== True) or ( i % 3 == 1 and elem.isSymbol == False):
			print("Error Code 60")
			sys.exit(60)

		if (i % 3 == 2 ):
			if (elem.pom[0:2] != charArrow) == True or (elem.isSymbol) == True :
				print("Error Code 60")
				sys.exit(60)
			else: elem.pom = str.lstrip(elem.pom[2:])

	regEx = re.compile(r'^[a-zA-Z](_?[a-zA-Z0-9]+)*$')
	regularExpression(s1, regEx)
	if regularExpression(s1, regEx) == False:
		print("Error Code 60")
		sys.exit(60)

	if len(s1.elements[3]) != 1 or s1.elements[3][0].isSymbol:
		print("Error Code 60")
		sys.exit(60)

	if mka_i == True:
		for element in s1.elements:
			for elem in element:
				elem.pom = elem.pom.lower()
#********************************************************************************************#
#Pomocna funkcia, ktora	napr. aj vymaze stav ktory treba
def stripStates(pom):
	s1 = Struct()

	for element in s1.elements:
		pomElement = []
		for elem in element:
			if ( elem.isSymbol == False):
				elem.pom = str.strip(elem.pom)
				if not elem.pom:
					pomElement.insert(len(pomElement), elem)
		if pomElement:
			for elem in pomElement:
				element.remove(elem)
		else: continue
#********************************************************************************************#
class Symbol(object):
	def __init__(self, name): self.name = name

	def __cmp__(self, other): cmp(self.name, other.name)

	def __eq__(self, other):
		if type(other) is str:
			if( self.name == other): return True
			else: return False
		else: return self is other

	def __lt__(self, other):
		if( self.name < other.name): return True
		else: return False

	def __str__(self): 
		pomChar = "'"
		return pomChar + (str(self.name)).replace(pomChar, "''") + pomChar
#********************************************************************************************#
class Pravidlo(object):
    def __init__(self, fromS, toS, pomSymbol):
        self.fromS = fromS
        self.toS = toS
        self.symbol = pomSymbol

    def __cmp__(self, other):
	    if (self.fromS == other.fromS):
	        if (self.symbol == other.symbol):
	            if (self.toS == other.toS): return 0
	            elif (self.toS < other.toS): return -1
	            else: return 1
	        elif (self.symbol < other.symbol): return -1
	        else: return 1
	    elif (self.fromS < other.fromS): return -1
	    else: return 1    

    def __eq__(self, other):
        if type(other) is Pravidlo:
        	if( self.fromS == other.fromS ):
        		if( self.symbol == other.symbol ):
        			if( self.toS == other.toS ): return True
        			else: return False
        		else: return False
        	else: return False
        else: return False

    def __lt__(self, other):
        if (self.fromS == other.fromS):
            if (self.symbol == other.symbol):
            	if( self.toS < other.toS ): return True
            	else: return False
            else:
            	if( self.symbol < other.symbol): return True
            	else: return False
        else:
        	if ( self.fromS < other.fromS ): return True
        	else: return False
       
    def __str__(self):
        return str(self.fromS) + " " + str(self.symbol) + " -> " + str(self.toS) 
#********************************************************************************************#
class Stav(object):
	def __init__(self, name): self.name = name

	def __cmp__(self, other): cmp(self.name, other.name)

	def __eq__(self, other):
		if type(other) is str:
			if( self.name == other): return True
			else: return False
		else: return self is other

	def __lt__(self, other):
		if( self.name < other.name): return True
		else: return False

	def __str__(self): return self.name
#********************************************************************************************#
class StateGroup(object):
    def __init__(self, stav):
        self.stav = []
        # Create copy of list
        for state in stav:
            self.stav.append(state)
            
    def __eq__(self, other):
        return self is other
            
    def __len__(self):
        return len(self.stav)
    
    def __iter__(self):
        for state in self.stav:
            yield state
            
    def __hash__(self):
        return hash(repr(self))
    
    def __str__(self):
        string = ""
        for state in self.stav[0:-1]:
            string += str(state) + "_"
        string += str(self.stav[-1])
        return string
#********************************************************************************************#
main()

#rozsirenie WHT --- stačí si načítať vstup a upraviť ho tak aby sa tam pridali:
# biely znak zamenit s ciarkou
# k stavom pridat apostrofy
"""
pom = ""
for line in arguments.mka_input:
        for char in line:
        	if char == '#'
        		pom = pom + '\n'
"""