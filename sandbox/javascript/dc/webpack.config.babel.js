import process from 'process';

import HtmlWebPackPlugin from 'html-webpack-plugin';
import MiniCssExtractPlugin from 'mini-css-extract-plugin';
import TerserPlugin from 'terser-webpack-plugin';
import OptimizeCSSAssetsPlugin from 'optimize-css-assets-webpack-plugin';
import DynamicCdnWebpackPlugin from 'dynamic-cdn-webpack-plugin';


const devMode = process.env.NODE_ENV !== 'production'


export default {
  entry: './src/index.jsx',
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
      // css
      {
        test: /\.s?css$/,
        use: [
          devMode ? 'style-loader' : MiniCssExtractPlugin.loader,
          'css-loader',
          'postcss-loader',
          'sass-loader',
        ],
      },
    ],
  },
  resolve: {
    extensions: ['.js', '.jsx']
  },
  plugins: [
    new HtmlWebPackPlugin({
      template: './src/html/index.html',
      // this uses the path related to output directory, not source directory
      filename: 'index.html',
    }),
    new MiniCssExtractPlugin({
      // Options similar to the same options in webpackOptions.output
      // both options are optional
      filename: devMode ? '[name].css' : '[name].css',
      chunkFilename: devMode ? '[id].css' : '[id].[hash].css',
    }),
    new DynamicCdnWebpackPlugin(),
  ],
  devtool: 'inline-source-map',
  optimization: {
    minimizer: [
      new TerserPlugin({
        parallel: true,
        terserOptions: {
          ecma: 6,
        },
      }),
      new OptimizeCSSAssetsPlugin({}),
    ],
  },
};
