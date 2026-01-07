## Webdriver
<table><tr><td> <strong><em> Automating Webdrivers </em></strong> </td></tr></table>

## Basic Example
```C

#include <driver-rfc2612/webdriver.h>
#include <stdlib.h>

int main(void) {
       Webdriver handle = webdriverChome(NULL); // Start session

        if (webdriver_Error(Handle)) {
            fprintf(stderr, webdriver_strerror());
            exit(EXIT_FAILURE);
        }
        handle.open("www.example.com");
        WebdriverClass element = handle->findElement(handle, By->ID, "box");
        element->sendKeys(element, "hello world");
        // or
        sendKeys(element, "closing")->action->scroll(element, "25px", "30px");

        //Screenshot window 
        handle->window->action(handle, Action->screenshot); // base64
        handle->window->action(handle, Action->screenshot, "jpg");

        // Make a request
        Webdriver_Request req = handle->httprequest(GET, "https://google.com/index.html");

        // Any connection?
        if ( webdriver_Error( handle->ping("8.8.8.8", 5) ))
           {
             // No connection 
           }
        
        Webdriver_Options opt;
        WebdriverChromeOptionsAdd(opt, "experimental");
        WebdriverChromeOptionsAdd(opt, WebdriverObject("excludeSwitches", (webdriver_Array)["--automation", "--logger"]));
        
        Webdriver_Service service;
        WebdriverServiceAdd(service, Service->port, 39675);
        Webdriver fromservice = webdriverChrome(service, opt);

        if (webdriver_Error(fromservice))
           {
              // handle error
           }
        WebdriverClose(handle); // Session is closed

        WebdriverQuit(handle); // quit driver instance
        WebdriverQuit(fromservice); // or WebdriverClose(service)

        return 0;
}
```
<br>
