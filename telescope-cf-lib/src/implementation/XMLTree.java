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

import static iface.Constants.DefaultArraySize;
import java.io.ByteArrayInputStream;
import javax.xml.parsers.DocumentBuilderFactory;
import javax.xml.parsers.DocumentBuilder;
import org.w3c.dom.Document;
import org.w3c.dom.NodeList;
import org.w3c.dom.Node;
import java.io.File;
import java.io.IOException;
import java.util.Arrays;
import java.util.logging.Level;
import java.util.logging.Logger;
import javax.xml.parsers.ParserConfigurationException;
import org.w3c.dom.NamedNodeMap;
import org.xml.sax.SAXException;

/**
 *
 * @author I829920
 */
public class XMLTree 
{
	private DocumentBuilderFactory dbFactory = null;
	private DocumentBuilder dBuilder = null;
	private Document doc = null;
	
public int setUpDOM()
{
	dbFactory = DocumentBuilderFactory.newInstance();
	
	if (dbFactory == null) return -1;
	try 
	{
		dBuilder = dbFactory.newDocumentBuilder();
	} catch (ParserConfigurationException ex) 
	{
		Logger.getLogger(XMLTree.class.getName()).log(Level.SEVERE, null, ex);
		return -1;
	}
	return 0;
}	


public NodeList getNodeList(File f)
{
	if (dBuilder == null || f == null) return null;
	
	if (!f.exists()) return null;

	try 
	{
		doc = dBuilder.parse(f);
	} catch (SAXException ex) 
	{
		Logger.getLogger(XMLTree.class.getName()).log(Level.SEVERE, null, ex);
	} catch (IOException ex) 
	{
		Logger.getLogger(XMLTree.class.getName()).log(Level.SEVERE, null, ex);
	}
	
	if (doc == null) return null;
	
	//optional, but recommended
	//read this - http://stackoverflow.com/questions/13786607/normalization-in-dom-parsing-with-java-how-does-it-work
	doc.getDocumentElement().normalize();
	
	NodeList nList = doc.getDocumentElement().getChildNodes();
		return nList;
}

public NodeList getNodeList(ByteArrayInputStream bais)
{
	if (dBuilder == null || bais == null) return null;
	
	//if (!bais.exists()) return null;

	try 
	{
		doc = dBuilder.parse(bais);
	} catch (SAXException ex) 
	{
		Logger.getLogger(XMLTree.class.getName()).log(Level.SEVERE, null, ex);
	} catch (IOException ex) 
	{
		Logger.getLogger(XMLTree.class.getName()).log(Level.SEVERE, null, ex);
	}
	
	if (doc == null) return null;
	
	//optional, but recommended
	//read this - http://stackoverflow.com/questions/13786607/normalization-in-dom-parsing-with-java-how-does-it-work
	doc.getDocumentElement().normalize();
	
	NodeList nList = doc.getDocumentElement().getChildNodes();
		return nList;
}	

public String getValueOfElementWithName(NodeList nl, char [] elementName)
{
	if (nl == null && elementName == null) return null;
	Node node, AttrNode = null;
	
	char [] element = new char[DefaultArraySize];
	String elval = null;
	
	for (int i = 0; i < nl.getLength(); i++) 
	{
		node = nl.item(i);

		if (node.getNodeType() == Node.ELEMENT_NODE)
		{
			//System.out.println("getElementWithValue(): Element Name = " + node.getNodeName());
			//System.out.println("getElementWithValue(): Element Value = " + node.getTextContent());
			//System.out.println("Element Value = " + node.getNodeValue());
			
			Arrays.fill(element, '\0');
			
			System.arraycopy(node.getNodeName().toCharArray(), 0, element, 0, node.getNodeName().toCharArray().length);

			if (Arrays.equals(elementName, element))
			{
				//return node.getTextContent();
				elval = node.getTextContent();
				break;
			}
				
			if (node.hasAttributes()) 
			{
				// get attributes names and values
				NamedNodeMap nodeMap = node.getAttributes();
 
				for (int j = 0; j < nodeMap.getLength(); j++) 
				{
					Arrays.fill(element, '\0');
					AttrNode = nodeMap.item(j);
					System.arraycopy(AttrNode.getNodeName().toCharArray(), 0, element, 0, AttrNode.getNodeName().toCharArray().length);
					
					if (Arrays.equals(elementName, element))
					{	
						return AttrNode.getNodeValue();
					}	
				}
			}
			
			if (node.hasChildNodes())// perform recursion if node has child nodes
			{	
				elval = this.getValueOfElementWithName(node.getChildNodes(), elementName);
				if (elval != null) return elval;
			}	
		}
	}	
		return elval;
}	
	
}
