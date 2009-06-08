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
                $( '#toggle' ).click( function() {
                    $( '.items' ).each( function() {
                        this.checked = !this.checked;
                    } );
                } );
            } );
        //]]></script>
        <title>Web shopping cart</title>
    </head>
    <body>
        <%@page import="org.assignment.Cart" %>
        <form name="cart" action="response.jsp" method="POST">
            <table border="1">
                <thead>
                    <tr>
                        <th><button id="toggle" type="button">Toggle</button></th>
                        <th>Price</th>
                        <th>Item</th>
                        <th>Brief</th>
                    </tr>
                </thead>
                <tbody>
                    <%
                    String checkbox = "<td><input type=\"checkbox\" class=\"items\" name=\"%s\" value=\"%d\" /></td>";
                    String text = "<td>%s</td>";
                    for( int i = 0; i < 20; ++i ) {
                        Integer price = Cart.randomValue(100, 99999);
                        String name = Cart.randomName( Cart.randomValue( 3, 10 ) );
                        out.print( "<tr>" );
                        out.print( String.format( checkbox, name, price ) );
                        out.print( String.format( text, price ) );
                        out.print( String.format( text, name ) );
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
