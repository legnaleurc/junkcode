<%@taglib prefix="c" uri="http://java.sun.com/jsp/jstl/core"%>
<%-- 
    Document   : response
    Created on : Jun 7, 2009, 12:03:49 AM
    Author     : legnaleurc
--%>

<%@page contentType="text/html" pageEncoding="UTF-8"%>
<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN"
   "http://www.w3.org/TR/html4/loose.dtd">

<html>
    <head>
        <meta http-equiv="Content-Type" content="text/html; charset=UTF-8">
        <title>Response</title>
    </head>
    <body>
        <jsp:useBean id="cart" scope="session" class="org.assignment.Cart" />
        <%
            out.println( request.getParameterMap() );
        %>
    </body>
</html>
