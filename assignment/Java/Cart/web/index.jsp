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
        <script type="text/javascript" src="jquery.js"></script>
        <script type="text/javascript">//<![CDATA[
            $( function() {
                $( '#increase' ).click( function() {
                    $( '.items' ).each( function() {
                        ++this.value;
                    } );
                } );
            } );
        //]]></script>
        <title>Web shopping cart</title>
    </head>
    <body>
        <%@page import="org.assignment.Cart" %>
        <%@page import="java.util.Hashtable" %>
        <%@page import="java.util.Map" %>
        <form name="cart" action="response.jsp" method="POST">
            <table border="1">
                <thead>
                    <tr>
                        <th><button id="increase" type="button">Increase</button></th>
                        <th>Price</th>
                        <th>Item</th>
                        <th>Brief</th>
                    </tr>
                </thead>
                <tbody>
                    <%
                    Hashtable< String, Integer > items = Cart.loadItems();
                    String input = "<td><input type=\"text\" class=\"items\" name=\"%s\" value=\"0\" /></td>";
                    String text = "<td>%s</td>";
                    for( Map.Entry< String, Integer > e : items.entrySet() ) {
                        out.print( "<tr>" );
                        out.print( String.format( input, e.getKey() ) );
                        out.print( String.format( text, e.getValue() ) );
                        out.print( String.format( text, e.getKey() ) );
                        out.print( String.format( text, Cart.randomBrief( Cart.randomValue( 1, 3 ) ) ) );
                        out.print( "</tr>" );
                    }
                    %>
                </tbody>
            </table>
            <input type="submit" />
        </form>
    </body>
</html>
