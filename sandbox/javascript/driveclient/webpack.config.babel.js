import HtmlWebPackPlugin from 'html-webpack-plugin';


export default {
  entry: './src/js/index.jsx',
  module: {
    rules: [
      // js, jsx
      {
        test: /\.jsx?$/,
        exclude: /node_modules/,
        use: {
          loader: 'babel-loader',
        },
      },
      // html
      {
        test: /\.html$/,
        use: {
          loader: 'html-loader',
        },
      },
    ],
  },
  plugins: [
    new HtmlWebPackPlugin({
      template: './src/html/index.html',
      // this uses the path related to output directory, not source directory
      filename: 'index.html',
    }),
  ],
};
