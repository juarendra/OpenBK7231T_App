

#include "../new_common.h"
#include "../logging/logging.h"
#include "ctype.h"
#include "new_http.h"
#include "http_fns.h"
#include "../new_pins.h"
#include "../new_cfg.h"
#include "../ota/ota.h"
#include "../hal/hal_wifi.h"
#include "../base64/base64.h"
#include "http_basic_auth.h"


// define the feature ADDLOGF_XXX will use
#define LOG_FEATURE LOG_FEATURE_HTTP

const char httpHeader[] = "HTTP/1.1 %d OK\nContent-type: %s";  // HTTP header
const char httpMimeTypeHTML[] = "text/html";              // HTML MIME type
const char httpMimeTypeText[] = "text/plain";           // TEXT MIME type
const char httpMimeTypeXML[] = "text/xml";           // TEXT MIME type
const char httpMimeTypeJson[] = "application/json";           // TEXT MIME type
const char httpMimeTypeBinary[] = "application/octet-stream";   // binary/file MIME type

const char htmlShortcutIcon[] = "<link rel='shortcut icon' href='data:image/gif;base64,R0lGODlhCwAQAPcAAAAAAAAAMwAAZgAAmQAAzAAA/wArAAArMwArZgArmQArzAAr/wBVAABVMwBVZgBVmQBVzABV/wCAAACAMwCAZgCAmQCAzACA/wCqAACqMwCqZgCqmQCqzACq/wDVAADVMwDVZgDVmQDVzADV/wD/AAD/MwD/ZgD/mQD/zAD//zMAADMAMzMAZjMAmTMAzDMA/zMrADMrMzMrZjMrmTMrzDMr/zNVADNVMzNVZjNVmTNVzDNV/zOAADOAMzOAZjOAmTOAzDOA/zOqADOqMzOqZjOqmTOqzDOq/zPVADPVMzPVZjPVmTPVzDPV/zP/ADP/MzP/ZjP/mTP/zDP//2YAAGYAM2YAZmYAmWYAzGYA/2YrAGYrM2YrZmYrmWYrzGYr/2ZVAGZVM2ZVZmZVmWZVzGZV/2aAAGaAM2aAZmaAmWaAzGaA/2aqAGaqM2aqZmaqmWaqzGaq/2bVAGbVM2bVZmbVmWbVzGbV/2b/AGb/M2b/Zmb/mWb/zGb//5kAAJkAM5kAZpkAmZkAzJkA/5krAJkrM5krZpkrmZkrzJkr/5lVAJlVM5lVZplVmZlVzJlV/5mAAJmAM5mAZpmAmZmAzJmA/5mqAJmqM5mqZpmqmZmqzJmq/5nVAJnVM5nVZpnVmZnVzJnV/5n/AJn/M5n/Zpn/mZn/zJn//8wAAMwAM8wAZswAmcwAzMwA/8wrAMwrM8wrZswrmcwrzMwr/8xVAMxVM8xVZsxVmcxVzMxV/8yAAMyAM8yAZsyAmcyAzMyA/8yqAMyqM8yqZsyqmcyqzMyq/8zVAMzVM8zVZszVmczVzMzV/8z/AMz/M8z/Zsz/mcz/zMz///8AAP8AM/8AZv8Amf8AzP8A//8rAP8rM/8rZv8rmf8rzP8r//9VAP9VM/9VZv9Vmf9VzP9V//+AAP+AM/+AZv+Amf+AzP+A//+qAP+qM/+qZv+qmf+qzP+q///VAP/VM//VZv/Vmf/VzP/V////AP//M///Zv//mf//zP///wAAAAAAAAAAAAAAACH5BAEAAPwALAAAAAALABAAAAiQANPAEZiGIMFMCHDgeKCQIYI3ahyk0aQpBw6KOQpS0uQAhwNNaToWBIUjh5qSy9IgKKhJoSaEFzX9KKgsx5uKF98IRPBjEpySmVQWxPFjDAIHOX7keFAwh7KJOJ6mwVEwzRsEyn7glJjmBzGQUTMtrdoSqSYECQtqPYqDGJxMCQTK6Li0pMU3bygNzIt3oJqAADs=' />";

const char htmlDoctype[] =
"<!DOCTYPE html><html>";
const char htmlHeadMeta[] =
"<meta charset=\"utf-8\">"
"<meta name=\"viewport\" content=\"width=device-width,initial-scale=1,user-scalable=no\">"
"<meta name=\"robots\" content=\"none\">";
const char htmlBodyStart[] =
"<body>"
"<noscript>To use this device, please enable JavaScript.<br></noscript>"
"<div id=\"main\">"
"<div class=\"header\" style=\"display: flex; flex-direction: row; align-items: center; justify-content: center;\">"
"<img src='data:image/png;base64,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' />"
"<h1>"
"<a target=\"_blank\" href=\"https://www.gspe.co.id/\">";
const char htmlBodyStart2[] =
"</a></h1></div>";
const char htmlBodyEnd[] = "</div></body></html>";

const char htmlFooterReturnToMainPage[] = "<a href=\"index\">Home</a>";
const char htmlFooterRefreshLink[] = "<a href=\"index\">Refresh</a>";
const char htmlFooterReturnToCfgOrMainPage[] =
"<a href=\"cfg\">Return to Config</a> | "
"<a href=\"index\">Home</a>";

const char htmlFooterInfo[] =
"<a target=\"_blank\" "
"href=\"https://www.gspe.co.id/\">Docs</a><br>";

const char* g_build_str = "Built on " __DATE__ " " __TIME__ " version " USER_SW_VER; // Show GIT version at Build line;

const char httpCorsHeaders[] = "Access-Control-Allow-Origin: *\r\nAccess-Control-Allow-Headers: Origin, X-Requested-With, Content-Type, Accept";           // TEXT MIME type

const char* methodNames[] = {
	"GET",
	"PUT",
	"POST",
	"OPTIONS"
};

#if WINDOWS
#define os_free free
#define os_malloc malloc
#endif

void misc_formatUpTimeString(int totalSeconds, char* o);

typedef struct http_callback_tag {
	char* url;
	int method;
	http_callback_fn callback;
	int auth_required;
} http_callback_t;

#define MAX_HTTP_CALLBACKS 32
static http_callback_t* callbacks[MAX_HTTP_CALLBACKS];
static int numCallbacks = 0;

int HTTP_RegisterCallback(const char* url, int method, http_callback_fn callback, int auth_required) {
	int i;

	if (!url || !callback) {
		return -1;
	}
	if (numCallbacks >= MAX_HTTP_CALLBACKS) {
		return -4;
	}
	for (i = 0; i < MAX_HTTP_CALLBACKS; i++) {
		if (callbacks[i]) {
			if (callbacks[i]->callback == callback && !strcmp(callbacks[i]->url, url)
				&& callbacks[i]->method == method) {
				return i;
			}
		}
	}
	callbacks[numCallbacks] = (http_callback_t*)os_malloc(sizeof(http_callback_t));
	if (!callbacks[numCallbacks]) {
		return -2;
	}
	callbacks[numCallbacks]->url = (char*)os_malloc(strlen(url) + 1);
	if (!callbacks[numCallbacks]->url) {
		os_free(callbacks[numCallbacks]);
		return -3;
	}
	strcpy(callbacks[numCallbacks]->url, url);
	callbacks[numCallbacks]->callback = callback;
	callbacks[numCallbacks]->method = method;
	callbacks[numCallbacks]->auth_required = auth_required > 0 ? 1 : 0;

	numCallbacks++;

	// success
	return 0;
}

int my_strnicmp(const char* a, const char* b, int len) {
	int i;
	for (i = 0; i < len; i++) {
		char x = *a;
		char y = *b;
		if (!x || !y) return 1;
		if ((x | 0x20) != (y | 0x20)) return 1;
		a++;
		b++;
	}
	return 0;
}


/// @brief Write escaped data to the response.
/// @param request
/// @param str
void poststr_escaped(http_request_t* request, char* str) {
	if (str == NULL) {
		postany(request, NULL, 0);
		return;
	}

	int i;
	bool foundChar = false;
	int len = strlen(str);

	//Do a quick check if escaping is necessary
	for (i = 0; (foundChar == false) && (i < len); i++) {
		switch (str[i]) {
		case '<':
			foundChar = true;
			break;
		case '>':
			foundChar = true;
			break;
		case '&':
			foundChar = true;
			break;
		case '"':
			foundChar = true;
			break;
		}
	}

	if (foundChar) {
		for (i = 0; i < len; i++) {
			switch (str[i]) {
			case '<':
				postany(request, "&lt;", 4);
				break;
			case '>':
				postany(request, "&gt;", 4);
				break;
			case '&':
				postany(request, "&amp;", 5);
				break;
			case '"':
				postany(request, "&quot;", 6);
				break;
			default:
				postany(request, str + i, 1);
				break;
			}
		}
	}
	else {
		postany(request, str, strlen(str));
	}
}

bool http_startsWith(const char* base, const char* substr) {
	while (*substr != 0) {
		if (*base != *substr)
			return false;
		if (*base == 0)
			return false;
		base++;
		substr++;
	}
	return true;
}

bool http_checkUrlBase(const char* base, const char* fileName) {
	while (*base != 0 && *base != '?' && *base != ' ') {
		if (*base != *fileName)
			return false;
		if (*base == 0)
			return false;
		base++;
		fileName++;
	}
	if (*fileName != 0)
		return false;
	return true;
}

void http_setup(http_request_t* request, const char* type) {
	hprintf255(request, httpHeader, request->responseCode, type);
	poststr(request, "\r\n"); // next header
	poststr(request, httpCorsHeaders);
#if 0
	poststr(request, "Server: Tasmota/10.1.0 (ESP8266EX)");
	poststr(request, "\r\n");
	poststr(request, "Cache-Control: no-cache, no-store, must-revalidate");
	poststr(request, "\r\n");
	poststr(request, "Pragma: no-cache");
	poststr(request, "\r\n");
	poststr(request, "Expires: -1");
	poststr(request, "\r\n");
	poststr(request, "Accept-Ranges: none");
	poststr(request, "\r\n");
	poststr(request, "Transfer-Encoding: chunked");
#endif
	poststr(request, "\r\n");
	poststr(request, "Connection: close");
	poststr(request, "\r\n"); // end headers with double CRLF
	poststr(request, "\r\n");
}

void http_html_start(http_request_t* request, const char* pagename) {
	poststr(request, htmlDoctype);
	poststr(request, "<head><title>");
	poststr(request, "GSPE_IOT");
	if (pagename) {
		hprintf255(request, " - %s", pagename);
	}
	poststr(request, "</title>");
	poststr(request, htmlShortcutIcon);
	poststr(request, htmlHeadMeta);
	poststr(request, htmlHeadStyle);
	poststr(request, "</head>");
	poststr(request, htmlBodyStart);
	poststr(request, " GSPE IOT ");
	poststr(request, CFG_GetDeviceName());
	
	poststr(request, htmlBodyStart2);
}

void http_html_end(http_request_t* request) {
	char upTimeStr[128];
	unsigned char mac[32];

	poststr(request, " | ");
	poststr(request, htmlFooterInfo);
	poststr(request, "<br>");
	poststr(request, g_build_str);

	hprintf255(request, "<br>Online for&nbsp;<span id=\"onlineFor\" data-initial=\"%i\">-</span>", g_secondsElapsed);

	WiFI_GetMacAddress((char*)mac);

	snprintf(upTimeStr, sizeof(upTimeStr), "<br>Device MAC: %02X:%02X:%02X:%02X:%02X:%02X", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
	poststr(request, upTimeStr);
	snprintf(upTimeStr, sizeof(upTimeStr), "<br>Smart Device IOT By Graha Sumber Prima Elektronik (GSPE)");
	poststr(request, upTimeStr);

	poststr(request, htmlBodyEnd);
	poststr(request, pageScript);
}

const char* http_checkArg(const char* p, const char* n) {
	while (1) {
		if (*n == 0 && (*p == 0 || *p == '='))
			return p;
		if (*p != *n)
			return 0;
		p++;
		n++;
	}
	return p;
}

int http_copyCarg(const char* atin, char* to, int maxSize) {
	int a, b;
	int realSize;
	const unsigned char* at = (unsigned char*)atin;

	realSize = 0;

	while (*at != 0 && *at != '&' && *at != ' ') {
#if 0
		* to = *at;
		to++;
		at++;
		maxSize--;
#else
		if ((*at == '%') &&
			((a = at[1]) && (b = at[2])) &&
			(isxdigit(a) && isxdigit(b))) {
			if (a >= 'a')
				a -= 'a' - 'A';
			if (a >= 'A')
				a -= ('A' - 10);
			else
				a -= '0';
			if (b >= 'a')
				b -= 'a' - 'A';
			if (b >= 'A')
				b -= ('A' - 10);
			else
				b -= '0';
			// can we afford to place this char in the target?
			if (maxSize > 1) {
				maxSize--;
				*to++ = 16 * a + b;
			}
			realSize++;
			at += 3;
		}
		else if (*at == '+') {
			// can we afford to place this char in the target?
			if (maxSize > 1) {
				maxSize--;
				*to++ = ' ';
			}
			realSize++;
			at++;
		}
		else {
			// can we afford to place this char in the target?
			if (maxSize > 1) {
				maxSize--;
				*to++ = *at;
			}
			realSize++;
			at++;
		}
#endif
	}
	*to = 0;
	return realSize;
}

int http_getRawArg(const char* base, const char* name, char* o, int maxSize) {
	*o = '\0';

	while (*base) {
		const char* at = http_checkArg(base, name);
		if (at) {
			at++;
			return http_copyCarg(at, o, maxSize);
		}
		while (*base != '&') {
			if (*base == 0) {
				return 0;
			}
			base++;
		}
		base++;
	}
	return 0;
}
int http_getArg(const char* base, const char* name, char* o, int maxSize) {
	*o = '\0';
	while (*base != '?') {
		if (*base == '\0')
			return 0;
		base++;
	}
	base++;

	return http_getRawArg(base, name, o, maxSize);
}
int http_getArgInteger(const char* base, const char* name) {
	char tmp[16];
	if (http_getArg(base, name, tmp, sizeof(tmp)) == 0)
		return 0;
	return atoi(tmp);
}

const char* htmlPinRoleNames[] = {
	" ",
	"Rel",
	"Rel_n",
	"Btn",
	"Btn_n",
	"LED",
	"LED_n",
	"PWM",
	"WifiLED",
	"WifiLED_n",
	"Btn_Tgl_All",
	"Btn_Tgl_All_n",
	"dInput",
	"dInput_n",
	"TglChanOnTgl",
	"dInput_NoPullUp",
	"dInput_NoPullUp_n",
	"BL0937SEL",
	"BL0937CF",
	"BL0937CF1",
	"ADC",
	"SM2135DAT",
	"SM2135CLK",
	"BP5758D_DAT",
	"BP5758D_CLK",
	"BP1658CJ_DAT",
	"BP1658CJ_CLK",
	"PWM_n",
	"IRRecv",
	"IRSend",
	"Btn_NextColor",
	"Btn_NextColor_n",
	"Btn_NextDimmer",
	"Btn_NextDimmer_n",
	"AlwaysHigh",
	"AlwaysLow",
	"UCS1912_DIN",
	"SM16703P_DIN",
	"Btn_NextTemperature",
	"Btn_NextTemperature_n",
	"Btn_ScriptOnly",
	"Btn_ScriptOnly_n",
	"DHT11",
	"DHT12",
	"DHT21",
	"DHT22",
	"CHT83XX_SDA",
	"CHT83XX_SCK",
	"SHT3X_SDA",
	"SHT3X_SCK",
	"SoftSDA",
	"SoftSCL",
	"SM2235DAT",
	"SM2235CLK",
	"BridgeFWD",
	"BridgeREV",
	"Btn_SmartLED",
	"Btn_SmartLED_n",
	"DoorSnsrWSleep",
	"DoorSnsrWSleep_nPup",
	"BAT_ADC",
	"BAT_Relay",
	"TM1637_DIO",
	"TM1637_CLK",
	"BL0937SEL_n",
	"DoorSnsrWSleep_pd",
	"SGP_CLK",
	"SGP_DAT",
	"ADC_Button",
	"GN6932_CLK",
	"GN6932_DAT",
	"GN6932_STB",
	"TM1638_CLK",
	"TM1638_DAT",
	"TM1638_STB",
	"BAT_Relay_n",
	"KP18058_CLK",
	"KP18058_DAT",
	"DS1820_IO",
	"error",
	"error",
	"error",
};

const char* PIN_RoleToString(int role) {
	return htmlPinRoleNames[role];
}
int PIN_ParsePinRoleName(const char* name) {
	int i;

	if (!stricmp(name, "None")) {
		return IOR_None;
	}

	for (i = 0; i < IOR_Total_Options; i++) {
		if (!stricmp(name, htmlPinRoleNames[i]))
			return i;
	}
	return IOR_Total_Options;
}

void setupAllWB2SPinsAsButtons() {
	PIN_SetPinRoleForPinIndex(6, IOR_Button);
	PIN_SetPinChannelForPinIndex(6, 1);

	PIN_SetPinRoleForPinIndex(7, IOR_Button);
	PIN_SetPinChannelForPinIndex(7, 1);

	PIN_SetPinRoleForPinIndex(8, IOR_Button);
	PIN_SetPinChannelForPinIndex(8, 1);

	PIN_SetPinRoleForPinIndex(23, IOR_Button);
	PIN_SetPinChannelForPinIndex(23, 1);

	PIN_SetPinRoleForPinIndex(24, IOR_Button);
	PIN_SetPinChannelForPinIndex(24, 1);

	PIN_SetPinRoleForPinIndex(26, IOR_Button);
	PIN_SetPinChannelForPinIndex(26, 1);

	PIN_SetPinRoleForPinIndex(27, IOR_Button);
	PIN_SetPinChannelForPinIndex(27, 1);
}

// add some more output safely, sending if necessary.
// call with str == NULL to force send. - can be binary.
// supply length
int postany(http_request_t* request, const char* str, int len) {
#if PLATFORM_BL602
	send(request->fd, str, len, 0);
	return 0;
#else
	int currentlen;
	int addlen = len;

	if (NULL == str) {
		// fd will be NULL for unit tests where HTTP packet is faked locally
		if (request->fd == 0) {
			return request->replylen;
		}
		if (request->replylen > 0) {
			send(request->fd, request->reply, request->replylen, 0);
		}
		request->reply[0] = 0;
		request->replylen = 0;
		return 0;
	}

	currentlen = request->replylen;
	if (currentlen + addlen >= request->replymaxlen) {
		send(request->fd, request->reply, request->replylen, 0);
		request->reply[0] = 0;
		request->replylen = 0;
		currentlen = 0;
	}
	while (addlen >= request->replymaxlen) {
		if (request->replylen > 0) {
			send(request->fd, request->reply, request->replylen, 0);
			request->replylen = 0;
		}
		send(request->fd, str, (request->replymaxlen - 1), 0);
		addlen -= (request->replymaxlen - 1);
		str += (request->replymaxlen - 1);

		rtos_delay_milliseconds(1);
	}

	memcpy(request->reply + request->replylen, str, addlen);
	request->replylen += addlen;
	return (currentlen + addlen);
#endif
}


// add some more output safely, sending if necessary.
// call with str == NULL to force send.
int poststr(http_request_t* request, const char* str) {
	if (str == NULL) {
		return postany(request, NULL, 0);
	}
	return postany(request, str, strlen(str));
}

int hprintf255(http_request_t* request, const char* fmt, ...) {
	va_list argList;
	//BaseType_t taken;
	char tmp[256];
	memset(tmp, 0, sizeof(tmp));
	va_start(argList, fmt);
	vsnprintf(tmp, 255, fmt, argList);
	va_end(argList);
	return postany(request, tmp, strlen(tmp));
}

int HUE_APICall(http_request_t* request);

int HTTP_ProcessPacket(http_request_t* request) {
	int i;
	char* p;
	char* headers;
	char* protocol;
	//int bChanged = 0;
	char* urlStr = "";
	char* recvbuf;

	if (request->received == 0) {
		ADDLOGF_ERROR("You gave request with NULL input");
		return 0;
	}
	request->method = -1;
	recvbuf = request->received;
	for (i = 0; i < sizeof(methodNames) / sizeof(*methodNames); i++) {
		if (http_startsWith(recvbuf, methodNames[i])) {
			urlStr = recvbuf + strlen(methodNames[i]) + 2; // skip method name plus space, plus slash
			request->method = i;
			break;
		}
	}
	if (request->method == -1) {
		ADDLOGF_ERROR("unsupported method %7s", recvbuf);
		return 0;
	}
	if (request->reply == 0) {
		ADDLOGF_ERROR("You gave request with NULL buffer");
		return 0;
	}

	if (request->method == HTTP_GET) {
		//ADDLOG_INFO(LOG_FEATURE_HTTP, "HTTP request\n");
	}
	else {
		//ADDLOG_INFO(LOG_FEATURE_HTTP, "Other request\n");
	}

	// if OPTIONS, return now - for CORS
	if (request->method == HTTP_OPTIONS) {
		http_setup(request, httpMimeTypeHTML);
		i = strlen(request->reply);
		return i;
	}

	// chop URL at space
	p = strchr(urlStr, ' ');
	if (p != 0) {
		if (*p) {
			*p = '\0';
			p++; // past space
		}
		else {
			ADDLOGF_ERROR("invalid request\n");
			return 0;
		}
	}

	request->url = urlStr;

	// protocol is next, termed by \r\n
	protocol = p;
	p = strchr(protocol, '\r');
	if (p != 0) {
		if (*p) {
			*p = '\0';
			p++; // past \r
			p++; // past \n
		}
		else {
			ADDLOGF_ERROR("invalid request\n");
			return 0;
		}
	}
	// i.e. not received
	request->contentLength = -1;
	headers = p;
	if (headers != 0) {
		do {
			p = strchr(headers, '\r');
			if (p != headers) {
				if (p) {
					if (request->numheaders < MAX_HEADERS) {
						request->headers[request->numheaders] = headers;
						request->numheaders++;
					}
					// pick out contentLength
					if (!my_strnicmp(headers, "Content-Length:", 15)) {
						request->contentLength = atoi(headers + 15);
					}

					*p = 0;
					p++; // past \r
					p++; // past \n
					headers = p;
				}
				else {
					break;
				}
			}
			if (*p == '\r') {
				// end of headers
				*p = 0;
				p++;
				p++;
				break;
			}
		} while (1);
	}

	if (p == 0) {
		request->bodystart = 0;
		request->bodylen = 0;
	}
	else {
		request->bodystart = p;
		request->bodylen = request->receivedLen - (p - request->received);
	}
#if 0
	postany(request, "test", 4);
	return 0;
#elif 0
	return http_fn_empty_url(request);
#endif

#if ENABLE_DRIVER_HUE
	if (HUE_APICall(request)) {
		return 0;
	}
#endif

	// look for a callback with this URL and method, or HTTP_ANY
	for (i = 0; i < numCallbacks; i++) {
		char* url = callbacks[i]->url;
		if (http_startsWith(urlStr, &url[1])) {
			int method = callbacks[i]->method;
			if (method == HTTP_ANY || method == request->method) {
				if (callbacks[i]->auth_required > 0 && http_basic_auth_run(request) == HTTP_BASIC_AUTH_FAIL) {
					return 0;
				}
				return callbacks[i]->callback(request);
			}
		}
	}

	if (http_basic_auth_run(request) == HTTP_BASIC_AUTH_FAIL) {
		return 0;
	}

	if (http_checkUrlBase(urlStr, "")) return http_fn_empty_url(request);

	if (http_checkUrlBase(urlStr, "testmsg")) return http_fn_testmsg(request);
	if (http_checkUrlBase(urlStr, "index")) return http_fn_index(request);

	if (http_checkUrlBase(urlStr, "about")) return http_fn_about(request);

	if (http_checkUrlBase(urlStr, "cfg_mqtt")) return http_fn_cfg_mqtt(request);
	if (http_checkUrlBase(urlStr, "cfg_ip")) return http_fn_cfg_ip(request);
	if (http_checkUrlBase(urlStr, "cfg_mqtt_set")) return http_fn_cfg_mqtt_set(request);

	if (http_checkUrlBase(urlStr, "cfg_webapp")) return http_fn_cfg_webapp(request);
	if (http_checkUrlBase(urlStr, "cfg_webapp_set")) return http_fn_cfg_webapp_set(request);

	if (http_checkUrlBase(urlStr, "cfg_wifi")) return http_fn_cfg_wifi(request);
	if (http_checkUrlBase(urlStr, "cfg_name")) return http_fn_cfg_name(request);
	if (http_checkUrlBase(urlStr, "cfg_wifi_set")) return http_fn_cfg_wifi_set(request);

	if (http_checkUrlBase(urlStr, "cfg_loglevel_set")) return http_fn_cfg_loglevel_set(request);
	if (http_checkUrlBase(urlStr, "cfg_mac")) return http_fn_cfg_mac(request);

//	if (http_checkUrlBase(urlStr, "flash_read_tool")) return http_fn_flash_read_tool(request);
	if (http_checkUrlBase(urlStr, "cmd_tool")) return http_fn_cmd_tool(request);
	if (http_checkUrlBase(urlStr, "startup_command")) return http_fn_startup_command(request);
	if (http_checkUrlBase(urlStr, "cfg_generic")) return http_fn_cfg_generic(request);
	if (http_checkUrlBase(urlStr, "cfg_startup")) return http_fn_cfg_startup(request);
	if (http_checkUrlBase(urlStr, "cfg_dgr")) return http_fn_cfg_dgr(request);

	if (http_checkUrlBase(urlStr, "ha_cfg")) return http_fn_ha_cfg(request);
	if (http_checkUrlBase(urlStr, "ha_discovery")) return http_fn_ha_discovery(request);
	if (http_checkUrlBase(urlStr, "cfg")) return http_fn_cfg(request);

	if (http_checkUrlBase(urlStr, "cfg_pins")) return http_fn_cfg_pins(request);
	if (http_checkUrlBase(urlStr, "cfg_ping")) return http_fn_cfg_ping(request);

	if (http_checkUrlBase(urlStr, "ota")) return http_fn_ota(request);
	if (http_checkUrlBase(urlStr, "ota_exec")) return http_fn_ota_exec(request);
	if (http_checkUrlBase(urlStr, "cm")) return http_fn_cm(request);

	return http_fn_other(request);
}

/*
NOTE:

The following fields should not be manually edited.
Instead, edit the script/css files in this folder and then re-generate the fields by running gulp.
Gulp tasks should automatically appear in Visual Code or can be invoked from console.
See https://github.com/openshwprojects/OpenBK7231T_App/blob/main/BUILDING.md for gulp setup.
*/

//region_start htmlHeadStyle
const char htmlHeadStyle[] = "<style>div,fieldset,input,select{padding:5px;font-size:1em;margin:0 0 .2em}fieldset{background:#4f4f4f}p{margin:.5em 0}input{width:100%;box-sizing:border-box;-webkit-box-sizing:border-box;-moz-box-sizing:border-box;background:#ddd;color:#000}form{margin-bottom:.5em}input[type=checkbox],input[type=radio]{width:1em;margin-right:6px;vertical-align:-1px}input[type=range]{width:99%}select{width:100%;background:#ddd;color:#000}textarea{resize:vertical;width:98%;height:318px;padding:5px;overflow:auto;background:#1f1f1f;color:#65c115}body{text-align:center;font-family:verdana,sans-serif}body,h1 a{background:#ffffff;color:#000000}td{padding:0}button,input[type=submit]{border:0;border-radius:.3rem;background:#06244a;color:#faffff;line-height:2.4rem;font-size:1.2rem;cursor:pointer}input[type=submit]{width:100%;transition-duration:.4s}input[type=submit]:hover{background:#0e70a4}.bred{background:#d43535!important}.bred:hover{background:#931f1f!important}.bgrn{background:#47c266!important}.bgrn:hover{background:#5aaf6f!important}a{color:#06244a;text-decoration:none}.p{float:left;text-align:left}.q{float:right;text-align:right}.r{border-radius:.3em;padding:2px;margin:6px 2px;background:linear-gradient(90deg,#ffa000,#a6d1ff)}.hf{display:none}.hdiv{width:95%;white-space:nowrap}.hele{width:210px;display:inline-block;margin-left:2px}div#state{padding:0}div#changed{padding:0;height:23px}div#main{text-align:left;display:inline-block;color:#000000;min-width:340px;max-width:800px}table{table-layout:fixed;width:100%}.disp-none{display:none}.disp-inline{display:inline-block}.safe{color:red}form.indent{padding-left:16px}li{margin:5px 0}.off,.on{text-align:center;font-size:54px}.on{font-weight:700}</style>";
//region_end htmlHeadStyle

//region_start pageScript
const char pageScript[] = "<script type='text/javascript'>var firstTime,lastTime,onlineFor,req=null,onlineForEl=null,getElement=e=>document.getElementById(e);function showState(){clearTimeout(firstTime),clearTimeout(lastTime),null!=req&&req.abort(),(req=new XMLHttpRequest).onreadystatechange=()=>{var e;4==req.readyState&&\"OK\"==req.statusText&&((\"INPUT\"!=document.activeElement.tagName||\"number\"!=document.activeElement.type&&\"color\"!=document.activeElement.type)&&(e=getElement(\"state\"))&&(e.innerHTML=req.responseText),clearTimeout(firstTime),clearTimeout(lastTime),lastTime=setTimeout(showState,3e3))},req.open(\"GET\",\"index?state=1\",!0),req.send(),firstTime=setTimeout(showState,3e3)}function fmtUpTime(e){var t,n,o=Math.floor(e/86400);return e%=86400,t=Math.floor(e/3600),e%=3600,n=Math.floor(e/60),e=e%60,0<o?o+` days, ${t} hours, ${n} minutes and ${e} seconds`:0<t?t+` hours, ${n} minutes and ${e} seconds`:0<n?n+` minutes and ${e} seconds`:`just ${e} seconds`}function updateOnlineFor(){onlineForEl.textContent=fmtUpTime(++onlineFor)}function onLoad(){(onlineForEl=getElement(\"onlineFor\"))&&(onlineFor=parseInt(onlineForEl.dataset.initial,10))&&setInterval(updateOnlineFor,1e3),showState()}function submitTemperature(e){var t=getElement(\"form132\");getElement(\"kelvin132\").value=Math.round(1e6/parseInt(e.value)),t.submit()}window.addEventListener(\"load\",onLoad),history.pushState(null,\"\",window.location.pathname.slice(1)),setTimeout(()=>{var e=getElement(\"changed\");e&&(e.innerHTML=\"\")},5e3);</script>";
//region_end pageScript

//region_start ha_discovery_script
const char ha_discovery_script[] = "<script type='text/javascript'>function send_ha_disc(){var e=new XMLHttpRequest;e.open(\"GET\",\"/ha_discovery?prefix=\"+document.getElementById(\"ha_disc_topic\").value,!1),e.onload=function(){200===e.status?alert(e.responseText):404===e.status&&alert(\"Error invoking ha_discovery\")},e.onerror=function(){alert(\"Error invoking ha_discovery\")},e.send()}</script>";
//region_end ha_discovery_script
