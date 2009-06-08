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
        <div>What you have selected:</div>
        <%@page import="org.assignment.Cart" %>
        <%@page import="java.util.Enumeration" %>
        <table border="1">
        <%
            Cart cart = new Cart();
            for( Enumeration< String > e = request.getParameterNames(); e.hasMoreElements(); ) {
                String key = e.nextElement();
                Integer value = Integer.parseInt( request.getParameter( key ) );
                out.print( String.format( "<tr><td>%s</td><td>%d</td></tr>", key, value ) );
                cart.addItem( key, value );
            }
        %>
        </table>
        <h1><%= cart.getTotalValue() %></h1>
        <a href="index.jsp">back and buy again</a>
    </body>
</html>
