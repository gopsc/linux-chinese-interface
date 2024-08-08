/*
 * @Author: 绪锋
 * @Date: 2024-06-20 19:34:13
 * @LastEditors: 绪锋
 * @LastEditTime: 2024-06-28 16:49:52
 * @FilePath: \note\src\store\index.js
 * @Description: 
 */
import { createStore } from 'vuex'

export default createStore({
  state: {
    menuInfo: {},
    childInfo: [],
    nodeList: [],
    currentNode: {}
  },
  mutations: {
    setCurrentNode (state, active) {
      state.currentNode = active
    },
    setMenuInfo(state, active) {
      state.menuInfo = active
    },
    setChildInfo (state, active) {
      state.childInfo = active
    },
    setNodeList (state, active) {
      state.nodeList = active
    }
  },
  actions: {
  }
})
