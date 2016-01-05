var path = require('path');
var webpack = require('webpack');

module.exports = {
  entry: './js/index.js',
  output: {
    path: __dirname,
    filename: './webapp/js/bundle.js',
  },
  // devtool: 'inline-source-map',
  module: {
    loaders: [
      {
        test: /.jsx?$/,
        loader: 'babel-loader',
        exclude: /node_modules/,
        query: {
          plugins: [
            'transform-es2015-modules-commonjs',
          ],
        },
      },
    ],
  },
};
