/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

import static iface.Constants.DefaultArraySize;
import implementation.Engine;
import java.util.Arrays;
import org.junit.After;
import org.junit.AfterClass;
import org.junit.Before;
import org.junit.BeforeClass;
import org.junit.Ignore;
import org.junit.Test;

/**
 *
 * @author I829920
 */
public class engineLibTest {
	
	public engineLibTest() {
	}
	
	@BeforeClass
	public static void setUpClass() {
	}
	
	@AfterClass
	public static void tearDownClass() {
	}
	
	@Before
	public void setUp() {
	}
	
	@After
	public void tearDown() {
	}

	// TODO add test methods here.
	// The methods must be annotated with annotation @Test. For example:
	//
	@Test
	public void hello() 
	{
		String XMLmessage = "<XML_MESSAGE length=\"00002120\" version=\"0.4\" xmlns=\"urn:ietf:params:xml:ns:xfb-0.4\" type_value=\"2\" type=\"UPDATE\"><PEERING as_num_len=\"2\"><SRC_ADDR><ADDRESS>129.82.138.6</ADDRESS><AFI value=\"1\">IPV4</AFI></SRC_ADDR><SRC_PORT>4321</SRC_PORT><SRC_AS>6447</SRC_AS><DST_ADDR><ADDRESS>89.149.178.10</ADDRESS><AFI value=\"1\">IPV4</AFI></DST_ADDR><DST_PORT>179</DST_PORT><DST_AS>3257</DST_AS><BGPID>0.0.0.0</BGPID></PEERING><ASCII_MSG length=\"79\"><MARKER length=\"16\">FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF</MARKER><UPDATE withdrawn_len=\"0\" path_attr_len=\"52\"><WITHDRAWN count=\"0\"/><PATH_ATTRIBUTES count=\"5\"><ATTRIBUTE length=\"1\"><FLAGS transitive=\"TRUE\"/><TYPE value=\"1\">ORIGIN</TYPE><ORIGIN value=\"0\">IGP</ORIGIN></ATTRIBUTE><ATTRIBUTE length=\"8\"><FLAGS transitive=\"TRUE\"/><TYPE value=\"2\">AS_PATH</TYPE><AS_PATH><AS_SEG type=\"AS_SEQUENCE\" length=\"3\"><AS>3257</AS><AS>6453</AS><AS>13156</AS></AS_SEG></AS_PATH></ATTRIBUTE><ATTRIBUTE length=\"4\"><FLAGS transitive=\"TRUE\"/><TYPE value=\"3\">NEXT_HOP</TYPE><NEXT_HOP>89.149.178.10</NEXT_HOP></ATTRIBUTE><ATTRIBUTE length=\"4\"><FLAGS optional=\"TRUE\"/><TYPE value=\"4\">MULTI_EXIT_DISC</TYPE><MULTI_EXIT_DISC>10</MULTI_EXIT_DISC></ATTRIBUTE><ATTRIBUTE length=\"20\"><FLAGS optional=\"TRUE\" transitive=\"TRUE\"/><TYPE value=\"8\">COMMUNITIES</TYPE><COMMUNITIES><COMMUNITY><AS>3257</AS><VALUE>8076</VALUE></COMMUNITY><COMMUNITY><AS>3257</AS><VALUE>30109</VALUE></COMMUNITY><COMMUNITY><AS>3257</AS><VALUE>50002</VALUE></COMMUNITY><COMMUNITY><AS>3257</AS><VALUE>51300</VALUE></COMMUNITY><COMMUNITY><AS>3257</AS><VALUE>51301</VALUE></COMMUNITY></COMMUNITIES></ATTRIBUTE></PATH_ATTRIBUTES><NLRI count=\"1\"><PREFIX label=\"NANN\"><ADDRESS>217.129.92.0/22</ADDRESS><AFI value=\"1\">IPV4</AFI><SAFI value=\"1\">UNICAST</SAFI></PREFIX></NLRI></UPDATE></ASCII_MSG><OCTET_MSG><OCTETS length=\"79\">FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF004F02000000344001010040020802030CB9193533644003045995B20A8004040000000AC008140CB91F8C0CB9759D0CB9C3520CB9C8640CB9C86516D9815C</OCTETS></OCTET_MSG></XML_MESSAGE>";
		
		
		String Query = "MULTI_EXIT_DISC	= 10 & SRC_AS =	6447 & DST_PORT = 179 & path_attr_len = 52";
		String Query0 = "type % AS_";
		String Query1 = "type = UPDATE";
		String Query2 = "SRC_AS = 6447";
		String Query3 = "MULTI_EXIT_DISC	= 10 | SRC_AS =	6447 | path_attr_len = 52";
		String Query4 = "SRC_AS =	6446 | path_attr_len = 51 | MULTI_EXIT_DISC	= 10";
		String Query5 = "SRC_AS =	6446 & path_attr_len = 51 & MULTI_EXIT_DISC	= 10";
		
		//complex expression should not be working in the current implementation
		String Query6 = "(SRC_AS = 6447  & MULTI_EXIT_DISC	= 10) |  (path_attr_len = 52  & type = UPDATE)";
		
		
		boolean result = false;
		
		Engine engine = new Engine();
		
		/*
		engine.addQuery(Query5);
		engine.addQuery(Query3);
		engine.addQuery(Query4);
		engine.addQuery(Query5);
		*/
		
		engine.addQuery(Query);
		
		result = engine.runQueries(XMLmessage);
		
		System.out.println("query returned " + result);
		
		engine.deleteQueries();
		
		
		engine.addQuery(Query0);
		
		result = engine.runQueries(XMLmessage);
		
		System.out.println("query returned " + result);
		
		engine.deleteQueries();
		
		
		
		engine.addQuery(Query1);
		
		result = engine.runQueries(XMLmessage);
		
		System.out.println("query returned " + result);
		
		engine.deleteQueries();
		
		
		engine.addQuery(Query2);
		
		result = engine.runQueries(XMLmessage);
		
		System.out.println("query returned " + result);
		
		engine.deleteQueries();
		
		
		
		engine.addQuery(Query3);
		
		result = engine.runQueries(XMLmessage);
		
		System.out.println("query returned " + result);
		
		engine.deleteQueries();
		
		
		
		engine.addQuery(Query4);
		
		result = engine.runQueries(XMLmessage);
		
		System.out.println("query returned " + result);
		
		engine.deleteQueries();
		
		
		
		engine.addQuery(Query5);
		
		result = engine.runQueries(XMLmessage);
		
		System.out.println("query returned " + result);
		
		engine.deleteQueries();
		
		
		
		engine.addQuery(Query6);
		
		result = engine.runQueries(XMLmessage);
		
		System.out.println("query returned " + result);
		
		engine.deleteQueries();
	
	}
	
	
	@Ignore
	public void testArrays() 
	{
		char [] str =  {'h', 'e', 'l', 'l', 'o'};
		
		char[][] elements;
		elements = new char[DefaultArraySize][DefaultArraySize];
		for (int i = 0; i < elements.length; i++)
			Arrays.fill(elements[i], '\0');
		
		System.arraycopy(str, 0,  elements[0], 0, str.length);
		System.arraycopy(str, 0,  elements[1], 0, str.length);
		
		System.out.println("str is:  " + str[0]);
		System.out.println("element is:  " + elements[0][0]);
		System.out.println("element is:  " + elements[1][0]);
		
		if (Arrays.equals(elements[0], elements[1])) System.out.println("element arrays are equal.");
		
		
	}
	
}
