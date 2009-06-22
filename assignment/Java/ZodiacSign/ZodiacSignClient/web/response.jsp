<%-- 
    Document   : response.jsp
    Created on : Jun 22, 2009, 5:24:07 PM
    Author     : legnaleurc
--%>

<%@page contentType="text/html" pageEncoding="UTF-8"%>
<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN"
   "http://www.w3.org/TR/html4/loose.dtd">

<html>
    <head>
        <meta http-equiv="Content-Type" content="text/html; charset=UTF-8">
        <title>JSP Page</title>
    </head>
    <body>
    <%-- start web service invocation --%><hr/>
    <%
    try {
	org.assignment.ZodiacService service = new org.assignment.ZodiacService();
	org.assignment.Zodiac port = service.getZodiacPort();
	 // TODO initialize WS operation arguments here
    int month = Integer.parseInt( request.getParameter("month") );
	int date = Integer.parseInt( request.getParameter("date") );
	// TODO process result here
	java.lang.String result = port.judge(month, date);
	out.println("Result = "+result);
    } catch (Exception ex) {
	// TODO handle custom exceptions here
    }
    %>
    <%-- end web service invocation --%><hr/>

    </body>
</html>
