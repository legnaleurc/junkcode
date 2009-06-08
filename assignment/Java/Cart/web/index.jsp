<%-- 
    Document   : index
    Created on : Jun 6, 2009, 2:36:52 PM
    Author     : legnaleurc
--%>

<%@page contentType="text/html" pageEncoding="UTF-8"%>
<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN"
   "http://www.w3.org/TR/html4/loose.dtd">

<html>
    <head>
        <meta http-equiv="Content-Type" content="text/html; charset=UTF-8">
        <title>Web shopping cart</title>
    </head>
    <body>
        <form name="cart" action="response.jsp" method="POST">
            <table border="1">
                <thead>
                    <tr>
                        <th></th>
                        <th>Price</th>
                        <th>Item</th>
                        <th>Brief</th>
                    </tr>
                </thead>
                <tbody>
<%
                    for( int i = 0; i < 20; ++i ) {
%>
                    <tr>
                        <td><input type="checkbox" name="item<%= i %>" value="100" /></td>
                        <td></td>
                        <td></td>
                        <td></td>
                    </tr>
<%
                        }
%>
                </tbody>
            </table>
            <input type="submit" />
        </form>
    </body>
</html>
