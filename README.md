#KT_WSO2_PHPCPP

WSO2 Web services Framework (WSF) for PHP (client only), rewrited with PHP CPP.  This is under developpement and can not be used for production. Lots of features are not yet backported due to lack of time and testing scenarii 

Important consideration, please keep in mind i'm not a C/C++ dev and there is certainly a lot of ugly things that must be fixed and/or refactored. ( PR are welcome ) 

The original WSO2 WSF for PHP extension was a abstract layer based on Axis2/c with some additional ( actually optional ) modules such as Rampart/c (WS-SEC), Sandesha2/c (WS-RM ) etc. 

In addtion to this abstract layer, WSO2 developped some PHP scripts handling serialize/unserialize helpers when users want to work in WSDL mode. ( and certainly others stuff ). I did not manage any backport of the old fashion scripts. If you planed to work w/ Object, I suggest to use JMS Serializer and pass the serialized JMS Object. 

The API is refactored but is almost similar to the native WSO2 extension. I will try to document it ASAP. 

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

### Policy
```
$policy_xml = <<<XML
<wsp:Policy>
    ... OASIS
</wsp:Policy>
XML;

$WSPolicy->setXMLPolicy($policy_xml);
```

###Security Token ( Username Token )
```
$WSSecToken->setUser("username")->setPassword("password");
```

###WSMessage
```
	$WSMessage
	->setEndpoint("http(S)://endpoint")
	->setPayload("<ws:calculate xmlns:ws=""></ws:calculate>");
```

###WSClient
```
$WSClient
->setSoapVersion(1.2)
->setMessage($WSMessage)
->setSecToken($WSSecToken)
->setPolicy($WSPolicy)
->setSSLServerCert("Absolute path to SSL CA"); //Mandatory if working w/ SSL
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
