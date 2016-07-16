#KT_WSO2_PHPCPP

WSO2 Web services Framework (WSF) for PHP (client only), rewrited with PHP CPP.  This is under developpement and can not be used for production. Lots of features are not yet backported due to lack of time and testing scenarii 

Important, please keep in mind i'm not a C/C++ dev and there is certainly a lot of ugly things that must be fixed and/or refactored. ( PR are welcome ) 

The original WSO2 WSF for PHP extension was a abstract layer for Axis2/c with some additional ( actually optional ) modules such as Rampart/c (WS-SEC), Sandesha2/c (WS-RM ) etc. 

In addtion to this abstract layer, WSO2 developped some PHP scripts handling serialize/unserialize helpers when users want to work in WSDL mode. ( and certainly others stuff ). I did not manage any backport of the old fashion scripts. If you planed to work w/ Object, I suggest to use JMS Serializer and pass the serialized JMS Object  to the `WSMessage::setPayload` method. However, i will investigate how to implement JMS serializer in a while

The API is refactored but is "almost" similar to the native WSO2 extension. I will try to document it ASAP. 

SoapFault are not thrown as KTWS\WSFault object ( formerly know as \WSFault ), still waiting support for custom exceptions from PHPCPP team. This could be normally compiled for PHP7 through the master branch of PHPCPP. 

## Installation

###  Axis2/c and co. 
Firstly clone the following repositories from my git repos :

- kt_axis2c-unofficial
- kt_rampart
- kt_sandesha2
- kt_savan

and follow the compilation instructions shipped for each one in the README.md. You will have a working axis2 environnement. 

### PHP 5.6 - Ubuntu 

```
sudo LC_ALL=en_US.UTF-8 add-apt-repository ppa:ondrej/php
sudo apt-get update
sudo apt-get install php5.6, php5.6-dev
```

### PHP-CPP
 
Then you must grab, compile and install PHP-CPP ( google it ). If you want to prevent compilation issue, checkout it into the following folder : 

```
sudo mkdir -p /opt/build/php-cpp-1.5.4
git clone phpcpp-legacy
make && sudo make install 
```

### KT_WSO2_PHPCPP 

TO BE Continued

## API

###Objects
```
$WSClient   = new KTWS\WSClient;
$WSMessage  = new KTWS\WSMessage;
$WSHeader   = new KTWS\WSHeader;
$WSHeader1  = new KTWS\WSHeader;
$WSSecToken = new KTWS\WSSecurityToken;
$WSPolicy   = new KTWS\WSPolicy;
```

### WSHeader

Soap header can be defined by a KTWS\WSHeader instance, it support nested headers through 
the setData() method. 

Soap Header(s) must be set by `WSMessage::setHeaders()` method, which support a array of unbound 
headers

```
$WSHeader  = new KTWS\WSHeader;
$WSMessage = new KTWS\WSMessage;

$WSHeader
->setNs("myNamespace")
->setPrefix("myPrefix")
->setName("MyHeaderName")
->setMustUnderstand(true)

//Int as the role : 0,1,3 <- check the WSO2 API
->setRole(1) 

Array of unbound KTWS\Header or string or xml
->setData();

$WSMessage->setHeaders([$WSHeader, $WSHeader1, $WSHeader2]);
```

According to your definition, Axis2 will produce for example a log similar to : 

```
<?xml version="1.0" encoding="UTF-8"?>
<myHeader:name xmlns:myHeader="header0" xmlns:soapenv="http://schemas.xmlsoap.org/soap/envelope/" soapenv:mustUnderstand="1" soapenv:actor="http://www.w3.org/2003/05/soap-envelope/role/none">
   <myHeader1:theHeader1 xmlns:myHeader1="header1" soapenv:actor="http://www.w3.org/2003/05/soap-envelope/role/none">data1</myHeader1:theHeader1>
   <myHeader2:theHeader2 xmlns:myHeader2="header2" soapenv:actor="http://www.w3.org/2003/05/soap-envelope/role/none">data2</myHeader2:theHeader2>
   <myHeader3:theHeader3 xmlns:myHeader3="header3" soapenv:actor="http://www.w3.org/2003/05/soap-envelope/role/none">data3</myHeader3:theHeader3>
   <myHeader4:theHeader4 xmlns:myHeader4="header4" soapenv:actor="http://www.w3.org/2003/05/soap-envelope/role/next">
      <ns4:theHeader5 xmlns:ns4="header5" soapenv:actor="http://www.w3.org/2003/05/soap-envelope/role/none">data5</ns4:theHeader5>
      <ns5:theHeader6 xmlns:ns5="header6">data6</ns5:theHeader6>
   </myHeader4:theHeader4>
</myHeader:name>
```

### Policy
```
$policy_xml = <<<XML
<wsp:Policy>
    ... OASIS
</wsp:Policy>
XML;

$WSPolicy->setXMLPolicy($policy_xml);
```

###Security Token 
`KTWS\SecurityToken` and `KTWS\WSPolicy` provide a nice API to deal with WS-Security. WS-Security relay heavily on Rampart/c and neethi/c, both are modules for axis2/c 

Example for Username Token 
```
$WSSecToken
->setUser("username")
->setPassword("password");
```
In order to use this scenario, you must implement a valid Policy through `KTWS\WSPolicy`

```
$my_policy = <<<XML
<wsp:Policy wsu:Id="RmPolicy" xmlns:wsp="http://schemas.xmlsoap.org/ws/2004/09/policy" xmlns:wsrm="http://schemas.xmlsoap.org/ws/2005/02/rm/policy" xmlns:sp="http://docs.oasis-open.org/ws-sx/ws-securitypolicy/200702" xmlns:sanc="http://ws.apache.org/sandesha2/c/policy" xmlns:wsu="http://docs.oasis-open.org/wss/2004/01/oasis-200401-wss-wssecurity-utility-1.0.xsd">
    <wsp:ExactlyOne>
        <wsp:All>
	<sp:TransportBinding>
                <wsp:Policy>
                </wsp:Policy>
            </sp:TransportBinding>
            <sp:SignedSupportingTokens>
                <wsp:Policy>
                    <sp:UsernameToken
                        sp:IncludeToken="http://docs.oasis-open.org/ws-sx/ws-securitypolicy/200702/IncludeToken/AlwaysToRecipient">
                        <wsp:Policy>
                            <sp:WssUsernameToken10 />
                        </wsp:Policy>
                    </sp:UsernameToken>
                </wsp:Policy>
            </sp:SignedSupportingTokens
        </wsrm:RMAssertion>
        </wsp:All>
    </wsp:ExactlyOne>
</wsp:Policy>
XML;

$WSPolicy->setXMLPolicy($my_policy);
```


###WSMessage

WSMessage handle the SOAP message that must be sent. 

`WSMessage::setPayload [string]` : expect a string as the soap xml payload *without* specifying the SoapEnvelope, SoapHeaders nor SoapBody ; This is handled internally by Axis2/c and by the parameters you provide through `WSPolicy`, `WSSecurityToken` or `WSHeaders`. You only focus on the business logic by serializing your objects.  

`WSMessage::setEndpoint` expect a string as the endpoint where reside the soap contract. 

Basic usage : 
```
$WSMessage = KTWS\WSMessage;

$WSMessage
->setEndpoint("http(S)://endpoint")
->setPayload("<ws:calculate xmlns:ws=""></ws:calculate>");
```

WS-Adressing is backported, but not yet tested on my side. 
To get the full documentation, please check the official WSO2 Documentation


```
$WSMessage = KTWS\WSMessage;

$WSMessage
->setEndpoint("http(S)://endpoint")
->setPayload("<ws:calculate xmlns:ws=""></ws:calculate>");

->setAction("remoteAction")
->setFrom([
	'address' => "http://.....",
	
	//Array of unbound wsa Endpoint reference  
	'referenceParameters' => [
		
		//
	    	'<wsa:EndpointReference xmlns:wsa="..." xmlns:fabrikam="...">
   		 	<wsa:Address>http://www.fabrikam123.example/acct</wsa:Address>
   		 	<wsa:PortType>fabrikam:InventoryPortType</wsa:PortType>
		</wsa:EndpointReference>',
		
		//
		'<wsa:EndpointReference xmlns:wsa="..." xmlns:fabrikam="...">
   		 	<wsa:Address>http://www.fabrikam123.example/acct</wsa:Address>
   		 	<wsa:PortType>fabrikam:InventoryPortType</wsa:PortType>
		</wsa:EndpointReference>'
		
	],
	//Array of unbound wsa metadatas
	'metadatas' => [
	]
])
```
This is the same for ReplyTo and Fault : 

```
->setReply([ same as above ])
->setFault([ still the same ])
```

One more thing regarding WS-Adressing, in order to use it you must set, at the KTWS\WSClient level, 
the following method :

```
$WSClient->setWSA([string])
```
This method expect : 

- "1.0"
- "submission"
- "disabled"

###WSClient
```
$WSClient

//Soap version 1.1 or 1.2
->setSoapVersion(1.2)

//Work w/ REST
->disableSoap()

//
->setMessage($WSMessage)
->setSecToken($WSSecToken)
->setPolicy($WSPolicy)

//Proxy
->setProxyHost("127.0.0.1")
->setProxyPort("8080")
->setProxyUsername("proxyUsername")
->setProxyPassword("proxyPassword")
->setProxyAuthType("proxyAuth")

//HTTP Auth
->setHTTPUsername("username")
->setHTTPPassword("pasword")
->setHTTPAuth("Basic");

//Timeout in sec
->setTimeout(30)

//Mandatory if working w/ SSL
->setSSLServerCert("Absolute path to SSL CA");
```

###Requesting

```
try {
	$WSClient->request();
	
	print_r($WSMessage->getResponse());
	
	//print_r($WSClient->getMessage());

} catch(\Exception $e) {
	echo $e->getMessage();
}
```

###WS-RM / Reliable Messaging

i still need to investigate why WSO2 team disabled this feature.
However, Sandesha2/c is loaded by default. 


#Credits 
 - Alexis Gruet <alexis.gruet@kroknet.com>
 - WSO2 team 
