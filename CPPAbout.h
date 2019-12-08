#pragma once

#include "XHTMLStatic.h"

// CPPAbout 对话框

class CPPAbout : public CPropertyPage
{
	DECLARE_DYNAMIC(CPPAbout)

public:
	CPPAbout();
	virtual ~CPPAbout();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_CPPAbout };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
	CXHTMLStatic htmlContent;
	const TCHAR * htmlStr = {
							_T("<b><font size=\"+8\" color=\"#ff9900\"><center>Kamac</center></font></b><br>")
							_T("<font size=\"+8\" color=\"#ff9900\"><center>Keyboard and mouse activity counter</center></font><br><br>")
							_T("<b>Call me Ishmael.</b> Some years ")
							_T("ago -- <font color=\"#0000FF\"> never ")
							_T("mind how long precisely</font> -- having ")
							_T("<font color=\"green\">little</font>")
							_T("<font color=\"red\"><sup>3</sup></font> ")
							_T("or no money in my purse, and nothing ")
							_T("particular to interest me on shore, I ")
							_T("thought I would sail about a little and ")
							_T("see the watery part of the world. It is ")
							_T("a way I have of <font color=\"red\"> ")
							_T("driving off the spleen</font>, ")
							_T("and regulating the circulation. Whenever ")
							_T("I find myself growing grim about the ")
							_T("mouth; whenever it is a damp, drizzly ")
							_T("<font color=\"aqua\" bgcolor=\"black\"> ")
							_T("November </font> in my soul; whenever I find ")
							_T("myself <strike>involuntarily</strike> pausing before ")
							_T("coffin warehouses, and bringing up the ")
							_T("rear of every funeral I meet; and ")
							_T("<u>especially</u> whenever my hypos get such an ")
							_T("upper hand of me, that it requires a ")
							_T("strong moral principle to prevent me ")
							_T("from deliberately stepping into the ")
							_T("street, and methodically knocking ")
							_T("people's hats off -- then, I account it ")
							_T("high time to get to sea as soon as I ")
							_T("can. \r\n\r\nThis is my substitute for pistol ")
							_T("and ball.<br><br> With a philosophical flourish ")
							_T("Cato throws himself upon his sword; I quietly take ")
							_T("to the ship. <i><font color=\"DarkGreen\">There is ")
							_T("nothing surprising in this.</font></i>  If they but ")
							_T("knew it, almost all men in their degree, ")
							_T("some time or other, cherish very nearly ")
							_T("the same feelings towards the ocean with ")
							_T("me.&copy;")
							_T("<br><font color=\"#C6D6FD\"><hr size=2></font><br>")
							_T("<a href=\"http://www.melville.org/hmmoby.htm\">")
							_T("<b><i><font size=\"+6\" color=\"#FF0099\">")
							_T("<center>Visit the Moby Dick Web Site  </center></font></i></b></a><br><br>")
	};



public:
	virtual BOOL OnInitDialog();
};
