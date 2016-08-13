// var path = require('path');
// var webpack = require('webpack');


module.exports = {
  entry: {
    index: './src/index.jsx',
    'gbf.user': './src/gbf.user.js',
  },
  output: {
    path: `${__dirname}/build/`,
    filename: '[name].js',
    publicPath: '/',
  },
  module: {
    loaders: [
      {
        test: /\.jsx?$/,
        loader: 'babel',
        exclude: /node_modules/,
        query: {
          presets: [
            'react',
          ],
          plugins: [
            'transform-es2015-modules-umd',
          ],
        },
      }
    ],
  },
};
