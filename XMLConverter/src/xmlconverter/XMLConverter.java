/*
Copyright (c) 2012-2013  Kirill Alexandrovich Belyaev
 * kirillbelyaev@yahoo.com
 * kirill@cs.colostate.edu
 * XML Converter for TeleScope - XML Message Stream Broker - Continuous Query (CQ) Platform
 * This work is licensed under the Creative Commons Attribution-NonCommercial 3.0 Unported License. 
 * To view a copy of this license, visit http://creativecommons.org/licenses/by-nc/3.0/ or send 
 * a letter to Creative Commons, 444 Castro Street, Suite 900, Mountain View, California, 94041, USA.
*/

package xmlconverter;

import java.io.BufferedReader;
import java.io.BufferedWriter;
import java.io.File;
import java.io.FileReader;
import java.io.FileWriter;
import java.io.IOException;

/**
 *
 * @author kirill
 */
public class XMLConverter {

	static boolean start = false;
	static boolean stop = false;

	static final String startTag =  "<XML_MESSAGE length=";
	static final String stopTag =  "</XML_MESSAGE>";
	static String Tag = null;
	
	/**
	 * @param args the command line arguments
	 */
	
	public static void main(String[] args) {

		File datain = null;
		File dataout = null;
		
		if (args.length != 3)
	    {	
	        System.out.println("illegal number of arguments! should be 3: "
			+ "\n"
			+ "1: Tag Name"
			+ "\n"
			+ "2: XML Input File Name"
			+ "\n"
			+ "3: XML Output File");	
	        System.exit(-1);
	    }
		
		Tag = args[0];
		datain = new File(args[1]);
		dataout = new File(args[2]);
		process(datain, dataout);
	}

	static public void process(File datain, File dataout) 
	{
		StringBuilder contents = null;
		String message = null;
		String totalLenString = null;
		Integer len = 0;
		Integer totalLen = 0;
		BufferedReader input = null;
		BufferedWriter output = null;
		
		try {
			input = new BufferedReader(new FileReader(datain));
			output = new BufferedWriter(new FileWriter(dataout));
			contents = new StringBuilder();

			String line = null;
				
			while ((line = input.readLine()) != null) {

					line = line.trim();
					
					if (line.equals("<"+Tag+">")) {
						start = true;
						//System.out.println("match1!");
					} else if (line.equals("</"+Tag+">")) {
						stop = true;
						//System.out.println("match2!");
					} else if (line.startsWith("<"+Tag+">") && line.endsWith("</"+Tag+">")) {
						//System.out.println("match3!");
						start = true;
						stop = true;
					} else if (line.startsWith("<"+Tag) )  {
						start = true;
					} else if (line.endsWith("</"+Tag+">") )  {
						stop = true;
					}


					if (start && stop) {
						
						contents.append(line);
						
						message = contents.toString();
						
						message = message.replace("<"+Tag, startTag);
						
						message = message.replace("</"+Tag+">", stopTag);
						
						len = message.length();
						totalLen = len + 10; //10 is "" plus 8 digits of the length
						
						totalLenString = buildLen(totalLen);
						
						if (totalLenString == null) return;
						
						message = message.replace(startTag, startTag + "\"" + totalLenString + "\"");
						
						output.write(message);
						output.write("\n");
						start = false;
						stop = false;
						contents.delete(0, contents.length());
						
					} else if (start) {
						
						contents.append(line);
						
					} else if (stop) {
						
						contents.append(line);
						
						message = contents.toString();
						
						message = message.replace("<"+Tag, startTag);
						
						message = message.replace("</"+Tag+">", stopTag);
						
						len = message.length();
						totalLen = len + 10; //10 is "" plus 8 digits of the length
						
						totalLenString = buildLen(totalLen);
						
						if (totalLenString == null) return;
						
						message = message.replace(startTag, startTag + "\"" + totalLenString + "\"");
						
						output.write(message);
						output.write("\n");
						start = false;
						stop = false;
						contents.delete(0, contents.length());
					}
					
				}//end of while loop
			
				input.close();
				output.close();
				
		} catch (IOException e) {
			System.out.println("IO exception! One of the files specified does not exist!");
			System.exit(-1);
		}
	}
	
	static public String buildLen(Integer l)
	{
		final String messlength = "00000000";
		char [] ml = messlength.toCharArray();
		char [] il = l.toString().toCharArray();
		
		if (il.length > ml.length) return null;
		
		for (int i = ml.length-1, j = il.length-1; i >= 0; i--, j--)
		{	
			ml[i] = il[j];
			if (j == 0) break;
		}
		
		return (new String (ml));
	}
}
