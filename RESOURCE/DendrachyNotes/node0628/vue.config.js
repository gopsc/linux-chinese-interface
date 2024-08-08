/*
 * @Author: mraing 910408228@qq.com
 * @Date: 2023-04-10 17:23:04
 * @LastEditors: 绪锋
 * @LastEditTime: 2024-06-21 09:22:13
 * @FilePath: \归档\vue.config.js
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */

const { defineConfig } = require('@vue/cli-service')

const path = require('path')

function resolve(dir) {
  return path.join(__dirname, dir)
}

// 定义请求地址
const BASE = {
  dev: 'http://10.219.88.58:9000',
}

module.exports = defineConfig({
  // outputDir: 'note',
  // publicPath: '/note',
  // 打开调试模式
  configureWebpack: {
    devtool: 'source-map'
  },
  transpileDependencies: true,
  chainWebpack: (config) => {
    config.resolve.alias
      .set('@', resolve('src'))
      .set('assets',resolve('src/assets'))
      .set('styles',resolve('src/assets/styles'))
  },
  devServer: {
    client: {
      // 编译错误时，取消全屏覆盖（建议关掉）
      overlay: false
    },
    open: true,
    // host: 'localhost',
    port: 8000,
    https: false,
    proxy: {
      '/api/v1': {
        target: BASE.dev,
        changOrigin: true,
        pathRewrite: {
          '^/api/v1': ''
        }
      }
    }
  }
})
