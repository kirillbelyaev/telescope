/*
Copyright (c) 2010-2014  Kirill Belyaev
 * kirillbelyaev@yahoo.com
 * kirill@cs.colostate.edu
 * TeleScope-CF-Lib - XML Content Filtering Library
 * This work is licensed under the Creative Commons Attribution-NonCommercial 3.0 Unported License. 
 * To view a copy of this license, visit http://creativecommons.org/licenses/by-nc/3.0/ or send 
 * a letter to Creative Commons, 444 Castro Street, Suite 900, Mountain View, California, 94041, USA.
*/

/*
"DION
The violent carriage of it
Will clear or end the business: when the oracle,
Thus by Apollo's great divine seal'd up,
Shall the contents discover, something rare
Even then will rush to knowledge. Go: fresh horses!
And gracious be the issue!"

Winter's Tale, Act 3, Scene 1. William Shakespeare
*/

package implementation;

import iface.CEPLib;
import static iface.Constants.*;
import java.io.ByteArrayInputStream;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.StringTokenizer;
import org.w3c.dom.NodeList;

/**
 *
 * @author I829920
 */

public class Engine implements CEPLib 
{
	//parsing engine globals
	private int [] truth;
	private char [] plogic;
	private char [][]expg;
	
	//arrays for holding parsed cmd search expressions
	private char tuple[][];
	private char logic[];
	private char[][] elements;
	private char[] operators;
	private char[][] values;
	
	private int MatchingMessagesCount = -1;
	private String Query = null;
	private ArrayList <String> Queries = null;

	//parsing engine globals
	private boolean g_and = true;
	private boolean g_or;
	private boolean complex = false;
	private int exp_gn;
	
	private XMLTree tree = new XMLTree();
	private ByteArrayInputStream bais = null;
	
	public Engine() 
	{
		if (this.setUpEngineArrays() != 0) return;
		if (tree.setUpDOM() != 0) return;
		//System.out.println("EngineState initialized.");
	}
	
private int setUpEngineArrays()
{	
		truth = new int[DefaultArraySize];
		plogic = new char [DefaultArraySize];
		expg = new char [DefaultArraySize][DefaultArraySize];
		
		tuple = new char [DefaultArraySize][DefaultArraySize];
		logic = new char[DefaultArraySize];
		elements = new char[DefaultArraySize][DefaultArraySize];
		operators = new char [DefaultArraySize];
		values = new char [DefaultArraySize][DefaultArraySize];
		
		Queries = new ArrayList <String> ();
		
		return 0;
}
	
	
private void clearEngineArrays()
{
    int i = 0;
	
    Arrays.fill(truth, 0);
    Arrays.fill(plogic, '\0');
    
    for (i = 0; i < expg.length; i++)
	Arrays.fill(expg[i], '\0');
    
    Arrays.fill(operators, '\0');
    Arrays.fill(logic, '\0');
    
    for (i = 0; i < elements.length; i++)
	Arrays.fill(elements[i], '\0');
    
    for (i = 0; i < values.length; i++)
	Arrays.fill(values[i], '\0');
    
    for (i = 0; i < tuple.length; i++)
	Arrays.fill(tuple[i], '\0');
}
	
private void resetMatch()
{
        this.MatchingMessagesCount = 0;
}

private void incrementMatch()
{
        this.MatchingMessagesCount++;
}

	@Override
	public int addQuery(String q)
{
	if (q == null || q.isEmpty()) return -1;
	this.Queries.add(q);
	return 0;
}

	@Override
public void deleteQueries()
{
	this.Queries.clear();
}		
	
	@Override 
public int initializeEngineWithQuery(String q) 
{       
        this.resetMatch();//02/17/13
        
        this.clearEngineArrays(); //05/10/12 clear the exp globals arrays
	
	if (q == null || q.isEmpty()) return -1;
	
	Query = q;
	Query = Query.trim();

	//case: if exp contains ()
	//fill the expg global arrays with data
	if (Query.indexOf("(") != -1 && Query.indexOf(")") != -1) 
	{
		complex = true;
	}
        else {//05/09/12 reset to false
		complex = false;
             }
	
	//terminate immediately if any of () parentheses are found - we now use query decomposition 
	//that eliminates the need for () construct
	if (Query.indexOf("(") != -1 || Query.indexOf(")") != -1) return -1;
            
	if (complex == false) if (this.fillEngineArrays(Query) != 0) return -1;	

	return 0;
}


private int checkOperatorValidity(char o)
{
	int v = -1;
	for (int i = 0; i < OperatorSet.length; i++)
		if (o == OperatorSet[i]) { v = 0; break; }
	
	return v;
}

private int checkLogicValidity(char o)
{
	int v = -1;
	for (int i = 0; i < LogicSet.length; i++)
		if (o == LogicSet[i] || o == '\0') { v = 0; break; }
	
	return v;
}	
	
private int fillEngineArrays(String q)
{
	if (q == null || q.isEmpty()) return -1;
	
	int l = 0;
	int ll = 1;
	int tn = 0;
	int i = 0;

	StringTokenizer st = new StringTokenizer(q);
	
	while (st.hasMoreTokens())//let us get rid of any space or tab character to correctly deliniate the tokens 
	{
		tn++;
		char [] token = st.nextToken().toCharArray();
		//System.out.println(token);
		System.arraycopy(token, 0, tuple[i], 0, token.length);
		token = null;
		i++;
	}	
	
	i = 0;

	while (i < tn) 
	{
		System.arraycopy(tuple[i], 0,  elements[l], 0, tuple[i].length);
		i++;
		operators[l] = tuple[i][0];
		
		//terminate if operator is not a valid operator
		//System.out.println("operator: " + operators[l]);
		if (this.checkOperatorValidity(operators[l]) != 0) return -1;
		
		i++;

		System.arraycopy(tuple[i], 0,  values[l], 0, tuple[i].length);
		i++;
		logic[ll] = tuple[i][0];
		
		//terminate if operator is not a valid operator
		//System.out.println("logic: " + logic[ll]);
		if (this.checkLogicValidity(logic[ll]) != 0) return -1;
		
		i++;

		l++;
		ll++;
	}
		logic[0] = logic[1];

		return 0;
}

@Override
public boolean runQueries (String message)
{
	if (message == null || message.isEmpty()) return false;
	
	for (int i = 0; i < this.Queries.size(); i++)
		if (this.initializeEngineWithQuery(this.Queries.get(i)) == 0)
		{	
			if (this.runQuery(message) == true) return true;
		}	else return false;   
	
	return false;
}

//generic preferred method to run cep query on xml message string
@Override
public boolean runQuery (String message)
{
	if (this.Query == null || this.Query.isEmpty() || message == null || message.isEmpty()) return false;
	
	byte[] buffer = null;
	buffer = message.getBytes();
	bais = new ByteArrayInputStream (buffer);
	
	if (bais == null) return false;
	
	NodeList nl = tree.getNodeList(bais);
	
	String val = null;
	
	if (nl == null) return false;
	
	int numberOfExpressions = 0;
	int cycle_and[] = new int[DefaultArraySize];
	int cycle_or[] = new int[DefaultArraySize];
	
	boolean oracle_and = true;
	boolean oracle_or = false;
	boolean oracle = false;
	boolean and_found = false;

	int and_count = 0; 
	int or_count = 0;
	int i = 0;

	//General case when complex = false
	//lets fill the logic arrays with zeroes initially
	Arrays.fill(cycle_and, 0);
	Arrays.fill(cycle_or, 0);

	//lets count the number of expressions
	while (this.logic[i] == '&' || this.logic[i] == '|')
	{	
		i++; numberOfExpressions++;
	}	
		
	if (numberOfExpressions == 0)
		if ((this.elements[0].length) != 0 && this.operators[0] != 0 && (this.values[0].length) != 0)
			numberOfExpressions = 1;
	
	//lets count the AND and OR
	for (i=0; i < numberOfExpressions; i++) 
	{
		if (this.logic[i] == '&')
			and_count++;
		if (this.logic[i] == '|')
			or_count++;
	}
	
	for (i=0; i < numberOfExpressions; i++)//loop through defined tags/attributes of the expression
	{	
		val = tree.getValueOfElementWithName(nl, this.elements[i]);
		
		if (val == null) break; 
		
		if (processElement(val, this.elements[i], this.operators[i], this.values[i]) == 0)
		{//operate on elements	
			if (this.logic[i] == '&') {
			cycle_and[i] = 1;
					     }
			if (this.logic[i] == '|') {
			cycle_or[i] = 1;
					     }
			if (numberOfExpressions == 1) oracle = true; //if there is just one exp
	        }
		
	}//end of for loop

	//the deciding logic happens here
	for (i=0; i < numberOfExpressions; i++) 
	{
		if (cycle_and[i] != 1) {
			oracle_and = false;
			break;
				       }
	}
	
	for (i=0; i < numberOfExpressions; i++) 
	{
		if (cycle_or[i] == 1) {
			oracle_or = true;
			break;
				      }
	}

	if (oracle_and == true || oracle_or == true || oracle == true)
	{
		this.incrementMatch(); //increase the matching count
		return true;	
	}//end of checking oracles
		return false;
}

//core processing function
int processElement(String nodeValue, char [] element, char operator, char [] value)
{
	if (nodeValue == null || value == null) return -1;
	
	int i, j;
	int numericValue, numericValueStated;
	boolean string = false;

	try
	{	
		numericValue = Integer.valueOf(nodeValue.trim());
	} catch (NumberFormatException nfe) { numericValue = -1;  string = true; }
	
	try
	{	
		numericValueStated = Integer.valueOf(String.valueOf(value).trim());
	} catch (NumberFormatException nfe) { numericValueStated = -1;  string = true; }
	
	char [] nv = new char[DefaultArraySize];
	Arrays.fill(nv, '\0');
	System.arraycopy(nodeValue.toCharArray(), 0, nv, 0, nodeValue.toCharArray().length);

	if (operator == '=') 
	{
		if (string == false) 
		{
			if (numericValue == numericValueStated) return(0);		    
			else return(-1);
		}
		if (string == true) 
		{
			if (Arrays.equals(nv, value)) return(0);
			else return(-1);
		}
	}
	
	if (operator == '>') 
	{
		if (numericValue > numericValueStated) return(0);		     
		else return(-1);		     
	}
	
	if (operator == '<') 
	{
		if (numericValue < numericValueStated) return(0);
		else return(-1);
	}
	
	if (operator == '!') 
	{
		if (string == false) 
		{
			if (numericValue != numericValueStated) return(0);
			else return(-1);
		}
		if (string == true) 
		{
			if (!Arrays.equals(nv, value)) return(0);
			else return(-1);     
                }
	}
	
	if (operator == '%') 
	{
		if (string == false) 
		{
			return(-1);
		}
		if (string == true) 
		{
			String s = new String (value);
			if ( nodeValue.trim().indexOf(s.trim()) != -1 ) return (0);
			else return(-1);
                }
	}

	return -1;	
}


//Diagnostic routines

public void printTupleArray()
{
	System.out.println("Printing tuple array:");
	for (int i = 0; i < this.tuple.length; i++)
		{
			System.out.println(tuple[i]);
		}	
	System.out.println("End of Printing tuple array:");
}

public void printOperatorArray()
{
	System.out.println("Printing operator array:");
	for (int i = 0; i < this.operators.length; i++)
		{
			System.out.println(operators[i]);
		}	
	System.out.println("End of Printing operator array:");
}

public void printLogicArray()
{
	System.out.println("Printing logic array:");
	for (int i = 0; i < this.logic.length; i++)
		{
			System.out.println(logic[i]);
		}	
	System.out.println("End of Printing logic array:");
}

public void printElementArray()
{
	System.out.println("Printing element array:");
	for (int i = 0; i < this.elements.length; i++)
		{
			System.out.println(this.elements[i]);
		}	
	System.out.println("End of Printing element array:");
}


public void printValueArray()
{
	System.out.println("Printing value array:");
	for (int i = 0; i < this.values.length; i++)
		{
			System.out.println(values[i]);
		}	
	System.out.println("End of Printing value array:");
}

	@Override
	public int getMatchingMessagesCount()
{
	return this.MatchingMessagesCount;
}	


}//end of class
