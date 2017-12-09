/*
* Copyright (c) 2014-2018  Kirill Belyaev
* kirillbelyaev@yahoo.com
* kirill@cs.colostate.edu
* TeleScope - XML Message Stream Broker/Replicator Platform
* This work is licensed under the Creative Commons Attribution-NonCommercial 3.0 Unported License.
* To view a copy of this license, visit http://creativecommons.org/licenses/by-nc/3.0/ or send
* a letter to Creative Commons, 444 Castro Street, Suite 900, Mountain View, California, 94041, USA.
*
* Author: Vignesh M. Pagadala
* Vignesh.Pagadala@colostate.edu
* Last update: December 2017
*
* This product includes software developed by the OpenSSL Project for use in the OpenSSL Toolkit
* (http://www.openssl.org/).
*
* File: broker_emulator.c
* ------------------------
* Valid and invalid test-vectors.
* 
* Compilation: 
* gcc -g -Wall -I/expat.2.1.0.11/build/native/include -L/expat.2.1.0.11/build/native/lib/v100/Win32/Release/dynamic/utf8 -I/usr/OpenSSL-Win32/include -L/usr/OpenSSL-Win32/lib broker_emulator.c Control_Message_Processor.c -o broker_emulator_o -lexpat -lssl -lcrypto
*/

#include "Control_Message_Processor_Globals.h"

int main()
{
	// Test vectors for control message processing.
	
	// 1. Valid message.
	char validMsg[] = "<CRL_MESSAGE Length =\"0000712\"><CONTROL_MESSAGE_LENGTH>0000712</CONTROL_MESSAGE_LENGTH><CONTROL_MESSAGE_ID>CM0002</CONTROL_MESSAGE_ID><ROOT_BROKER_ID>192.168.100.1:50000</ROOT_BROKER_ID><CHILD_BROKER_ID>192.168.102.1:55550</CHILD_BROKER_ID><UBL><BROKER1 IP = \"129.82.47.138\" Port = \"80\" NameID = \"alpha\" Priority = \"60\"></BROKER1><BROKER2 IP = \"129.82.47.234\" Port = \"8080\" NameID = \"beta\" Priority = \"10\"></BROKER2><BROKER3 IP = \"129.82.47.242\" Port = \"5050\" NameID = \"gamma\" Priority = \"5\"></BROKER3><BROKER4 IP = \"129.82.47.230\" Port = \"8888\" NameID = \"delta\" Priority = \"0\"></BROKER4></UBL><FILTERING_QUERY>Industry = IND40</FILTERING_QUERY><SHA1>bae3ef3b79489e75fc191657050cc271aa1a069c</SHA1></CRL_MESSAGE>";
	printf("\n1. Valid message:\n %s\n\n", validMsg);
	printf("\n%d", process(validMsg));
	printf("\n\n");
	printUBL();
	printf("Upstream Brokers List: \n%s", get_UBL());
	printf("Filtering Query: %s", get_FQ());
	printf("\n\n");
	
	// 2. Invalid message: Has incorrect SHA-1 digest.
	char invalidMsg[] = "<CRL_MESSAGE Length =\"0000712\"><CONTROL_MESSAGE_LENGTH>0000712</CONTROL_MESSAGE_LENGTH><CONTROL_MESSAGE_ID>CM0002</CONTROL_MESSAGE_ID><ROOT_BROKER_ID>192.168.100.1:50000</ROOT_BROKER_ID><CHILD_BROKER_ID>192.168.102.1:55550</CHILD_BROKER_ID><UBL><BROKER1 IP = \"129.82.47.138\" Port = \"80\" NameID = \"alpha\" Priority = \"60\"></BROKER1><BROKER2 IP = \"129.82.47.234\" Port = \"8080\" NameID = \"beta\" Priority = \"10\"></BROKER2><BROKER3 IP = \"129.82.47.242\" Port = \"5050\" NameID = \"gamma\" Priority = \"5\"></BROKER3><BROKER4 IP = \"129.82.47.230\" Port = \"8888\" NameID = \"delta\" Priority = \"0\"></BROKER4></UBL><FILTERING_QUERY>Industry = IND40</FILTERING_QUERY><SHA1>a14f51c827d71674d746a30dd68ae32fba8f5c36</SHA1></CRL_MESSAGE>";
	printf("\n2. Invalid message (incorrect SHA-1 digest):\n %s\n\n", invalidMsg);
	printf("\n%d", process(invalidMsg));
	printf("\n\n");
	printf("Upstream Brokers List: \n%s", get_UBL());
	printf("Filtering Query: %s", get_FQ());
	printf("\n\n");

	// 3. Invalid message: Not well-formed - bad ending.
	char invalidMsg2[] = "<CONTROL_MESSAGE_LENGTH>0000660</CONTROL_MESSAGE_LENGTH><CONTROL_MESSAGE_ID>CM0002</CONTROL_MESSAGE_ID><ROOT_BROKER_ID>192.168.100.1:50000</ROOT_BROKER_ID><CHILD_BROKER_ID>192.168.102.1:55550</CHILD_BROKER_ID><UBL><BROKER1 IP = \"129.82.47.138\" Port = \"80\" NameID = \"alpha\" Priority = \"60\"></BROKER1><BROKER2 IP = \"129.82.47.234\" Port = \"8080\" NameID = \"beta\" Priority = \"10\"></BROKER2><BROKER3 IP = \"129.82.47.242\" Port = \"5050\" NameID = \"gamma\" Priority = \"5\"></BROKER3><BROKER4 IP = \"129.82.47.230\" Port = \"8888\" NameID = \"delta\" Priority = \"0\"></BROKER4></UBL><FILTERING_QUERY>Industry = IND40</FILTERING_QUERY><SHA1>a14f51c827d71674d746a30dd68ae32fba8f5c36";
	printf("\n3. Invalid message (not well-formed with bad ending):\n %s\n\n", invalidMsg2);
	printf("\n%d", process(invalidMsg2));
	printf("\n\n");
	printf("Upstream Brokers List: \n%s", get_UBL());
	printf("Filtering Query: %s", get_FQ());
	printf("\n\n");

	// 4. Invalid message: Not well-formed - tag imbalance (FILTERING_QUERY's closing tag is missing). Has correct digest.
	char invalidMsg3[] = "<CRL_MESSAGE Length =\"0000694\"><CONTROL_MESSAGE_LENGTH>0000694</CONTROL_MESSAGE_LENGTH><CONTROL_MESSAGE_ID>CM0002</CONTROL_MESSAGE_ID><ROOT_BROKER_ID>192.168.100.1:50000</ROOT_BROKER_ID><CHILD_BROKER_ID>192.168.102.1:55550</CHILD_BROKER_ID><UBL><BROKER1 IP = \"129.82.47.138\" Port = \"80\" NameID = \"alpha\" Priority = \"60\"></BROKER1><BROKER2 IP = \"129.82.47.234\" Port = \"8080\" NameID = \"beta\" Priority = \"10\"></BROKER2><BROKER3 IP = \"129.82.47.242\" Port = \"5050\" NameID = \"gamma\" Priority = \"5\"></BROKER3><BROKER4 IP = \"129.82.47.230\" Port = \"8888\" NameID = \"delta\" Priority = \"0\"></BROKER4></UBL><FILTERING_QUERY>Industry = IND40<SHA1>630db5c09736e02f55dd9c734dd92b549ba9e27e</SHA1></CRL_MESSAGE>";	
	printf("\n4. Invalid message (not well-formed with open/close tag imbalance):\n %s\n\n", invalidMsg3);
	printf("\n%d", process(invalidMsg3));
	printf("\n\n");
	printf("Upstream Brokers List: \n%s", get_UBL());
	printf("Filtering Query: %s", get_FQ());
	printf("\n\n");

	// 5. Invalid message: Has missing elements. In the following test-vector, 'ROOT_BROKER_ID' is missing. Has correct digest.
	char invalidMsg4[] = "<CRL_MESSAGE Length =\"0000660\"><CONTROL_MESSAGE_LENGTH>0000660</CONTROL_MESSAGE_LENGTH><CONTROL_MESSAGE_ID>CM0002</CONTROL_MESSAGE_ID><CHILD_BROKER_ID>192.168.102.1:55550</CHILD_BROKER_ID><UBL><BROKER1 IP = \"129.82.47.138\" Port = \"80\" NameID = \"alpha\" Priority = \"60\"></BROKER1><BROKER2 IP = \"129.82.47.234\" Port = \"8080\" NameID = \"beta\" Priority = \"10\"></BROKER2><BROKER3 IP = \"129.82.47.242\" Port = \"5050\" NameID = \"gamma\" Priority = \"5\"></BROKER3><BROKER4 IP = \"129.82.47.230\" Port = \"8888\" NameID = \"delta\" Priority = \"0\"></BROKER4></UBL><FILTERING_QUERY>Industry = IND40</FILTERING_QUERY><SHA1>5abc754869306894bcf34a3c519df2eebc8d21f9</SHA1></CRL_MESSAGE>";
	printf("\n5. Invalid message (missing elements - ROOT_BROKER_ID missing):\n %s\n\n", invalidMsg4);
	printf("\n%d", process(invalidMsg4));
	printf("\n\n");
	printf("Upstream Brokers List: \n%s", get_UBL());
	printf("Filtering Query: %s", get_FQ());
	printf("\n\n");

	// 6. Invalid message: Contents of some elements (except UBL and FQ) are empty. In the following example, CHILD_BROKER_ID's contents are missing. Has correct digest.
	char invalidMsg5[] = "<CRL_MESSAGE Length =\"0000693\"><CONTROL_MESSAGE_LENGTH>0000693</CONTROL_MESSAGE_LENGTH><CONTROL_MESSAGE_ID>CM0002</CONTROL_MESSAGE_ID><ROOT_BROKER_ID>192.168.100.1:50000</ROOT_BROKER_ID><CHILD_BROKER_ID></CHILD_BROKER_ID><UBL><BROKER1 IP = \"129.82.47.138\" Port = \"80\" NameID = \"alpha\" Priority = \"60\"></BROKER1><BROKER2 IP = \"129.82.47.234\" Port = \"8080\" NameID = \"beta\" Priority = \"10\"></BROKER2><BROKER3 IP = \"129.82.47.242\" Port = \"5050\" NameID = \"gamma\" Priority = \"5\"></BROKER3><BROKER4 IP = \"129.82.47.230\" Port = \"8888\" NameID = \"delta\" Priority = \"0\"></BROKER4></UBL><FILTERING_QUERY>Industry = IND40</FILTERING_QUERY><SHA1>ecb8f43d78806629dedc8c045d0cf2688fca0219</SHA1></CRL_MESSAGE>";
	printf("\n6. Invalid message (empty elements - CHILD_BROKER_ID empty):\n %s\n\n", invalidMsg5);
	printf("\n%d", process(invalidMsg5));
	printf("\n\n"); printf("Upstream Brokers List: \n%s", get_UBL());
	printf("Filtering Query: %s", get_FQ());
	printf("\n\n");

	// 7. Valid message.
	char validMsg2[] = "<CRL_MESSAGE Length =\"0000718\"><CONTROL_MESSAGE_LENGTH>0000718</CONTROL_MESSAGE_LENGTH><CONTROL_MESSAGE_ID>ID0032</CONTROL_MESSAGE_ID><ROOT_BROKER_ID>192.148.540.110:67400</ROOT_BROKER_ID><CHILD_BROKER_ID>122.148.122.132:57850</CHILD_BROKER_ID><UBL><BROKER1 IP = \"119.82.47.138\" Port = \"8040\" NameID = \"alpha\" Priority = \"60\"></BROKER1><BROKER2 IP = \"129.82.47.234\" Port = \"8080\" NameID = \"beta\" Priority = \"10\"></BROKER2><BROKER3 IP = \"129.82.47.242\" Port = \"5050\" NameID = \"gamma\" Priority = \"5\"></BROKER3><BROKER4 IP = \"129.82.47.230\" Port = \"8888\" NameID = \"delta\" Priority = \"0\"></BROKER4></UBL><FILTERING_QUERY>Industry = IND35</FILTERING_QUERY><SHA1>86971d62f20746200de3797f896e7620945ca40f</SHA1></CRL_MESSAGE>";
	printf("\n7. Valid message:\n %s\n\n", validMsg2);
	printf("\n%d", process(validMsg2));
	printf("\n\n");
	printf("Upstream Brokers List: \n%s", get_UBL());
	printf("Filtering Query: %s", get_FQ());
	printf("\n\n");

	// 8. Invalid message: Message is an XML data message, not a control message.
	char invalidMsg6[] = "<XML_MESSAGE length=\"00003190\" symbol=\"ABT\"><Ask/><AverageDailyVolume>4638280</AverageDailyVolume><Bid/><AskRealtime>42.66</AskRealtime><BidRealtime>40.00</BidRealtime><BookValue>15.696</BookValue><Change_PercentChange>+0.58 - +1.41%</Change_PercentChange><Change>+0.58</Change><Commission/><Currency>USD</Currency><ChangeRealtime>+0.58</ChangeRealtime><AfterHoursChangeRealtime>N/A - N/A</AfterHoursChangeRealtime><DividendShare>0.80</DividendShare><LastTradeDate>10/3/2014</LastTradeDate><TradeDate/><EarningsShare>1.537</EarningsShare><ErrorIndicationreturnedforsymbolchangedinvalid/><EPSEstimateCurrentYear>2.23</EPSEstimateCurrentYear><EPSEstimateNextYear>2.46</EPSEstimateNextYear><EPSEstimateNextQuarter>0.68</EPSEstimateNextQuarter><DaysLow>41.275</DaysLow><DaysHigh>41.86</DaysHigh><YearLow>32.75</YearLow><YearHigh>44.20</YearHigh><HoldingsGainPercent>- - -</HoldingsGainPercent><AnnualizedGain/><HoldingsGain/><HoldingsGainPercentRealtime>N/A - N/A</HoldingsGainPercentRealtime><HoldingsGainRealtime/><MoreInfo>cnsprmiIed</MoreInfo><OrderBookRealtime/><MarketCapitalization>62.840B</MarketCapitalization><MarketCapRealtime/><EBITDA>4.508B</EBITDA><ChangeFromYearLow>+9.04</ChangeFromYearLow><PercentChangeFromYearLow>+27.60%</PercentChangeFromYearLow><LastTradeRealtimeWithTime>N/A - &lt;b&gt;41.79&lt;/b&gt;</LastTradeRealtimeWithTime><ChangePercentRealtime>N/A - +1.41%</ChangePercentRealtime><ChangeFromYearHigh>-2.41</ChangeFromYearHigh><PercebtChangeFromYearHigh>-5.45%</PercebtChangeFromYearHigh><LastTradeWithTime>Oct  3 - &lt;b&gt;41.79&lt;/b&gt;</LastTradeWithTime><LastTradePriceOnly>41.79</LastTradePriceOnly><HighLimit/><LowLimit/><DaysRange>41.275 - 41.86</DaysRange><DaysRangeRealtime>N/A - N/A</DaysRangeRealtime><FiftydayMovingAverage>42.4775</FiftydayMovingAverage><TwoHundreddayMovingAverage>40.6536</TwoHundreddayMovingAverage><ChangeFromTwoHundreddayMovingAverage>+1.1364</ChangeFromTwoHundreddayMovingAverage><PercentChangeFromTwoHundreddayMovingAverage>+2.80%</PercentChangeFromTwoHundreddayMovingAverage><ChangeFromFiftydayMovingAverage>-0.6875</ChangeFromFiftydayMovingAverage><PercentChangeFromFiftydayMovingAverage>-1.62%</PercentChangeFromFiftydayMovingAverage><Name>Abbott Laboratori</Name><Notes/><Open>41.41</Open><PreviousClose>41.21</PreviousClose><PricePaid/><ChangeinPercent>+1.41%</ChangeinPercent><PriceSales>2.84</PriceSales><PriceBook>2.63</PriceBook><ExDividendDate>Jul 11</ExDividendDate><PERatio>26.81</PERatio><DividendPayDate>Nov 15</DividendPayDate><PERatioRealtime/><PEGRatio>1.74</PEGRatio><PriceEPSEstimateCurrentYear>18.48</PriceEPSEstimateCurrentYear><PriceEPSEstimateNextYear>16.75</PriceEPSEstimateNextYear><Symbol>ABT</Symbol><SharesOwned/><ShortRatio>2.90</ShortRatio><LastTradeTime>4:00pm</LastTradeTime><TickerTrend>&amp;nbsp;=-+===&amp;nbsp;</TickerTrend><OneyrTargetPrice>44.38</OneyrTargetPrice><Volume>3430933</Volume><HoldingsValue/><HoldingsValueRealtime/><YearRange>32.75 - 44.20</YearRange><DaysValueChange>- - +1.41%</DaysValueChange><DaysValueChangeRealtime>N/A - N/A</DaysValueChangeRealtime><StockExchange>NYSE</StockExchange><DividendYield>1.94</DividendYield><PercentChange>+1.41%</PercentChange></XML_MESSAGE>";
	printf("\n8. Invalid message (message is an XML data message, not a control message):\n %s\n\n", invalidMsg6);
	printf("\n%d", process(invalidMsg6));
	printf("\n\n");
	printf("Upstream Brokers List: \n%s", get_UBL());
	printf("Filtering Query: %s", get_FQ());
	printf("\n\n");

	// 9. Invalid message: Improper convention - element names not capitalized (Control_Message_Length).
	char invalidMsg7[] = "<CRL_MESSAGE Length =\"0000712\"><Control_Message_Length>0000712</Control_Message_Length><CONTROL_MESSAGE_ID>CM0002</CONTROL_MESSAGE_ID><ROOT_BROKER_ID>192.168.100.1:50000</ROOT_BROKER_ID><CHILD_BROKER_ID>192.168.102.1:55550</CHILD_BROKER_ID><UBL><BROKER1 IP = \"129.82.47.138\" Port = \"80\" NameID = \"alpha\" Priority = \"60\"></BROKER1><BROKER2 IP = \"129.82.47.234\" Port = \"8080\" NameID = \"beta\" Priority = \"10\"></BROKER2><BROKER3 IP = \"129.82.47.242\" Port = \"5050\" NameID = \"gamma\" Priority = \"5\"></BROKER3><BROKER4 IP = \"129.82.47.230\" Port = \"8888\" NameID = \"delta\" Priority = \"0\"></BROKER4></UBL><FILTERING_QUERY>Industry = IND40</FILTERING_QUERY><SHA1>438efdfac9c1a25ed143ffa9ae31bdff2917231b</SHA1></CRL_MESSAGE>";
	printf("\n9. Invalid message (improper convention - element names not capitalized):\n %s\n\n", invalidMsg7);
	printf("\n%d", process(invalidMsg7));
	printf("\n\n");
	printf("Upstream Brokers List: \n%s", get_UBL());
	printf("Filtering Query: %s", get_FQ());
	printf("\n\n");

	// 10. Valid message with 10 brokers.
	char validMsg3[] = "<CRL_MESSAGE Length =\"0001221\"><CONTROL_MESSAGE_LENGTH>0001221</CONTROL_MESSAGE_LENGTH><CONTROL_MESSAGE_ID>CM0002</CONTROL_MESSAGE_ID><ROOT_BROKER_ID>192.168.100.1:50000</ROOT_BROKER_ID><CHILD_BROKER_ID>192.168.102.1:55550</CHILD_BROKER_ID><UBL><BROKER1 IP = \"129.82.47.138\" Port = \"80\" NameID = \"alpha\" Priority = \"60\"></BROKER1><BROKER2 IP = \"129.82.47.234\" Port = \"8080\" NameID = \"beta\" Priority = \"10\"></BROKER2><BROKER3 IP = \"129.82.47.242\" Port = \"5050\" NameID = \"gamma\" Priority = \"5\"></BROKER3><BROKER4 IP = \"129.82.47.230\" Port = \"8888\" NameID = \"delta\" Priority = \"0\"></BROKER4><BROKER5 IP = \"119.32.45.260\" Port = \"8438\" NameID = \"rho\" Priority = \"02\"></BROKER5><BROKER6 IP = \"149.84.46.280\" Port = \"88\" NameID = \"phi\" Priority = \"07\"></BROKER6><BROKER7 IP = \"149.44.45.280\" Port = \"8658\" NameID = \"eta\" Priority = \"10\"></BROKER7><BROKER8 IP = \"159.84.66.280\" Port = \"8800\" NameID = \"nabla\" Priority = \"07\"></BROKER8><BROKER9 IP = \"149.64.46.280\" Port = \"88\" NameID = \"omega\" Priority = \"13\"></BROKER9><BROKER10 IP = \"149.84.46.241\" Port = \"88\" NameID = \"mu\" Priority = \"07\"></BROKER10></UBL><FILTERING_QUERY>Industry = IND40</FILTERING_QUERY><SHA1>dc843823b4d0d2b02337e35276c72e2d505e17f4</SHA1></CRL_MESSAGE>";
	printf("\n11. Valid message: more brokers - 10.\n %s\n\n", validMsg3);
	printf("\n%d", process(validMsg3));
	printf("\n\n");
	printf("Upstream Brokers List: \n%s", get_UBL());
	printf("Filtering Query: %s", get_FQ());
	printf("\n\n");

	// 11. Valid message with 2 brokers.
	char validMsg4[] = "<CRL_MESSAGE Length =\"0000540\"><CONTROL_MESSAGE_LENGTH>0000540</CONTROL_MESSAGE_LENGTH><CONTROL_MESSAGE_ID>CM0002</CONTROL_MESSAGE_ID><ROOT_BROKER_ID>192.168.100.1:50000</ROOT_BROKER_ID><CHILD_BROKER_ID>192.168.102.1:55550</CHILD_BROKER_ID><UBL><BROKER1 IP = \"129.82.47.138\" Port = \"80\" NameID = \"alpha\" Priority = \"60\"></BROKER1><BROKER2 IP = \"129.82.47.234\" Port = \"8080\" NameID = \"beta\" Priority = \"10\"></BROKER2></UBL><FILTERING_QUERY>Industry = IND40</FILTERING_QUERY><SHA1>d558df2fcd1cf4361dd4f32297612559a7759ee9</SHA1></CRL_MESSAGE>";
	printf("\n12. Valid message: 2 brokers.\n %s\n\n", validMsg4);
	printf("\n%d", process(validMsg4));
	printf("\n\n");
	printf("Upstream Brokers List: \n%s", get_UBL());
	printf("Filtering Query: %s", get_FQ());
	printf("\n\n");

	// 12. Invalid message: Incorrect length in CONTROL_MESSAGE_LENGTH element.
	char invalidMsg9[] = "<CRL_MESSAGE Length =\"0000172\"><CONTROL_MESSAGE_LENGTH>0000172</CONTROL_MESSAGE_LENGTH><CONTROL_MESSAGE_ID>CM0002</CONTROL_MESSAGE_ID><ROOT_BROKER_ID>192.168.100.1:50000</ROOT_BROKER_ID><CHILD_BROKER_ID>192.168.102.1:55550</CHILD_BROKER_ID><UBL><BROKER1 IP = \"129.82.47.138\" Port = \"80\" NameID = \"alpha\" Priority = \"60\"></BROKER1><BROKER2 IP = \"129.82.47.234\" Port = \"8080\" NameID = \"beta\" Priority = \"10\"></BROKER2><BROKER3 IP = \"129.82.47.242\" Port = \"5050\" NameID = \"gamma\" Priority = \"5\"></BROKER3><BROKER4 IP = \"129.82.47.230\" Port = \"8888\" NameID = \"delta\" Priority = \"0\"></BROKER4></UBL><FILTERING_QUERY>Industry = IND40</FILTERING_QUERY><SHA1>b46222e289b446e87d8632eadf17dd512f67f37c</SHA1></CRL_MESSAGE>";
	printf("\n13. Invalid message (Incorrect length in CONTROL_MESSAGE_LENGTH element):\n %s\n\n", invalidMsg9);
	printf("\n%d", process(invalidMsg9));
	printf("\n\n");
	printf("Upstream Brokers List: \n%s", get_UBL());
	printf("Filtering Query: %s", get_FQ());
	printf("\n\n");

	// 13. Invalid message: Incorrect length in Length attribute of CRL_MESSAGE element.
	char invalidMsg10[] = "<CRL_MESSAGE Length =\"0000702\"><CONTROL_MESSAGE_LENGTH>0000712</CONTROL_MESSAGE_LENGTH><CONTROL_MESSAGE_ID>CM0002</CONTROL_MESSAGE_ID><ROOT_BROKER_ID>192.168.100.1:50000</ROOT_BROKER_ID><CHILD_BROKER_ID>192.168.102.1:55550</CHILD_BROKER_ID><UBL><BROKER1 IP = \"129.82.47.138\" Port = \"80\" NameID = \"alpha\" Priority = \"60\"></BROKER1><BROKER2 IP = \"129.82.47.234\" Port = \"8080\" NameID = \"beta\" Priority = \"10\"></BROKER2><BROKER3 IP = \"129.82.47.242\" Port = \"5050\" NameID = \"gamma\" Priority = \"5\"></BROKER3><BROKER4 IP = \"129.82.47.230\" Port = \"8888\" NameID = \"delta\" Priority = \"0\"></BROKER4></UBL><FILTERING_QUERY>Industry = IND40</FILTERING_QUERY><SHA1>eb473c1274176406edcca3b6b7138a19d0132da9</SHA1></CRL_MESSAGE>";
	printf("\n14. Invalid message (Incorrect length in Length attribute of CRL_MESSAGE element):\n %s\n\n", invalidMsg10);
	printf("\n%d", process(invalidMsg10));
	printf("\n\n");
	printf("Upstream Brokers List: \n%s", get_UBL());
	printf("Filtering Query: %s", get_FQ());
	printf("\n\n");

	freeMem();

	/*
	 * Unit tests for SHA-1 validation.
	*/

	// 1. Authentic message. 
	char authMsg1[] = "<CRL_MESSAGE Length =\"0000712\"><CONTROL_MESSAGE_LENGTH>0000500</CONTROL_MESSAGE_LENGTH><CONTROL_MESSAGE_ID>CM0002</CONTROL_MESSAGE_ID><ROOT_BROKER_ID>192.168.100.1:50000</ROOT_BROKER_ID><CHILD_BROKER_ID>192.168.102.1:55550</CHILD_BROKER_ID><UBL><BROKER1 IP = \"129.82.47.138\" Port = \"80\" NameID = \"alpha\" Priority = \"60\"></BROKER1><BROKER2 IP = \"129.82.47.234\" Port = \"8080\" NameID = \"beta\" Priority = \"10\"></BROKER2><BROKER3 IP = \"129.82.47.242\" Port = \"5050\" NameID = \"gamma\" Priority = \"5\"></BROKER3><BROKER4 IP = \"129.82.47.230\" Port = \"8888\" NameID = \"delta\" Priority = \"0\"></BROKER4></UBL><FILTERING_QUERY>Industry = IND40</FILTERING_QUERY><SHA1>24861d5efc325064ef27e0c9c86cbb174b5da2ca</SHA1></CRL_MESSAGE>";
	char shamsg[] = "24861d5efc325064ef27e0c9c86cbb174b5da2ca";
	if (strcmp(shafunc(authMsg1), shamsg) == 0)
	{
		printf("The message \n\n %s \n\n is valid.\n\n\n", authMsg1);
	}
	else
	{
		printf("The message \n\n %s \n\n is invalid.\n\n\n", authMsg1);
	}

	// 2. Forged message.
	char forgedMsg1[] = "<CRL_MESSAGE Length =\"0000712\"><CONTROL_MESSAGE_LENGTH>0000500</CONTROL_MESSAGE_LENGTH><CONTROL_MESSAGE_ID>CM0002</CONTROL_MESSAGE_ID><ROOT_BROKER_ID>192.168.100.1:50000</ROOT_BROKER_ID><CHILD_BROKER_ID>192.168.102.1:55550</CHILD_BROKER_ID><UBL><BROKER1 IP = \"129.82.47.138\" Port = \"80\" NameID = \"alpha\" Priority = \"60\"></BROKER1><BROKER2 IP = \"129.82.47.234\" Port = \"8080\" NameID = \"beta\" Priority = \"10\"></BROKER2><BROKER3 IP = \"129.82.47.242\" Port = \"5050\" NameID = \"gamma\" Priority = \"5\"></BROKER3><BROKER4 IP = \"129.82.47.230\" Port = \"8888\" NameID = \"delta\" Priority = \"0\"></BROKER4></UBL><FILTERING_QUERY>Industry = IND40</FILTERING_QUERY><SHA1>a14f51c827d71674d746a30dd68ae32fba8f5c36</SHA1></CRL_MESSAGE>";
	char shamsg2[] = "a14f51c827d71674d746a30dd68ae32fba8f5c36";
	if (strcmp(shafunc(forgedMsg1), shamsg2) == 0)
	{
		printf("The message \n\n %s \n\n is valid.\n\n\n", forgedMsg1);
	}
	else
	{
		printf("The message \n\n %s \n\n is invalid.\n\n\n", forgedMsg1);
	}

	// 3. Authentic message.
	char authMsg2[] = "<CRL_MESSAGE Length =\"0000712\"><CONTROL_MESSAGE_LENGTH>0005130</CONTROL_MESSAGE_LENGTH><CONTROL_MESSAGE_ID>ID0032</CONTROL_MESSAGE_ID><ROOT_BROKER_ID>192.148.540.110:67400</ROOT_BROKER_ID><CHILD_BROKER_ID>122.148.122.132:57850</CHILD_BROKER_ID><UBL><BROKER1 IP = \"119.82.47.138\" Port = \"8040\" NameID = \"alpha\" Priority = \"60\"></BROKER1><BROKER2 IP = \"129.82.47.234\" Port = \"8080\" NameID = \"beta\" Priority = \"10\"></BROKER2><BROKER3 IP = \"129.82.47.242\" Port = \"5050\" NameID = \"gamma\" Priority = \"5\"></BROKER3><BROKER4 IP = \"129.82.47.230\" Port = \"8888\" NameID = \"delta\" Priority = \"0\"></BROKER4></UBL><FILTERING_QUERY>Industry = IND35</FILTERING_QUERY><SHA1>4636b2c09d5d9cd10fbe25923def03c88e840e66</SHA1></CRL_MESSAGE>";
	char shamsg3[] = "4636b2c09d5d9cd10fbe25923def03c88e840e66";
	if (strcmp(shafunc(authMsg2), shamsg3) == 0)
	{
		printf("The message \n\n %s \n\n is valid.\n\n\n", authMsg2);
	}
	else
	{
		printf("The message \n\n %s \n\n is invalid.\n\n\n", authMsg2);
	}
	
	getchar();
	return 0;
}
