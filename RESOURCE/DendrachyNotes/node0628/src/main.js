/*
 * @Author: 绪锋
 * @Date: 2024-06-20 19:34:13
 * @LastEditors: 绪锋
 * @LastEditTime: 2024-06-20 22:51:57
 * @FilePath: /note/src/main.js
 * @Description: 
 */
import { createApp } from 'vue'
import App from './App.vue'
import router from './router'
import store from './store'

import Varlet from '@varlet/ui'
import '@varlet/ui/es/style'

const app = createApp(App)
app.use(store)
app.use(router)
app.use(Varlet)
app.mount('#app')
