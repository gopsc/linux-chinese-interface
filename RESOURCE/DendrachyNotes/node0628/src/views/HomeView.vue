
<!--
 * @Author: 绪锋
 * @Date: 2024-06-20 19:34:13
 * @LastEditors: 绪锋
 * @LastEditTime: 2024-06-21 11:47:53
 * @FilePath: \归档\src\views\HomeView.vue
 * @Description: 
-->
<template>
  <div class="home">
    <var-app-bar class="header" title="目录" title-position="center"  />
    <var-button type="primary" round class="btn-plus" @click="show = true;">
      <var-icon name="plus" />
    </var-button>
    <div class="cell-list">
      <div
        v-for="item in menusList"
        :key="item.id"
        class="item ripple-example-block var-elevation--2"
        v-ripple
        @click="handleToChild(item)"
      >{{item.name}}</div>
    </div>
    <!-- 新增 -->
    <var-dialog v-model:show="show" @confirm="handleConfirm">
      <template #title>新增目录</template>
      <var-form 
        ref="form" 
        scroll-to-error="start"
      >
        <var-space direction="column" :size="[14, 0]">
          <var-input
            placeholder="请输入节点名称"
            :rules="[v => !!v || '名称不能为空']"
            v-model="formData.name"
          />
        </var-space>
        <var-space direction="column" :size="[14, 0]">
          <var-input
            placeholder="请输入节点信息"
            :rules="[v => !!v || '内容不能为空']"
            v-model="formData.title"
          />
        </var-space>
      </var-form>
    </var-dialog>
  </div>
</template>

<script setup>
import { onMounted, ref } from 'vue'
import { Snackbar } from '@varlet/ui'
import { useRouter } from 'vue-router'
import { useStore } from 'vuex'

import axios from 'axios';

const qs = require('qs')
const store = useStore()
const router = useRouter()

onMounted(() => {
  getMenuList()
})

const show = ref(false)
const formData = ref({
  name: '',
  title: ''
})
// 查询目录列表
const menusList = ref([])
const getMenuList = () => {
  formData.value = {
    name: '',
    title: ''
  }
  axios.get('/api/v1/queryByParent/', {})
    .then(res => {
      menusList.value = res.data
    })
}

// 新增节点
const handleConfirm = () => {
  axios({
    url: '/api/v1/addBook/' ,
    method: 'post',
    headers: {
      'Content-Type': 'application/json'
    },
    params:{
      name: formData.value.name,
      title: formData.value.title,
      pid: 0
    }
  })
    .then(res => {
      Snackbar.success(res.data)
      getMenuList()
    })
}

// 跳转到子目录
const handleToChild = (item) => {
  router.push({path: '/about'})
  store.commit('setNodeList', [item])
}
</script>

<style lang="stylus" scoped>
.ripple-example-block
  display: flex;
  user-select: none;
  cursor: pointer;
  -webkit-tap-highlight-color: transparent;
  transition: background-color 0.25s;
  background-color: var(--color-surface-container-highest);

.home
  position: relative
  width: 100vw
  height: 100vh
  display: flex
  flex-direction: column
  .btn-plus
    position: absolute
    bottom: var(--fab-bottom)
    right: var(--fab-left)
    width: 56px
    height: 56px
    z-index 9
  .cell-list
    padding: 10px 0
    flex: 1
    overflow-y: scroll
    .item
      padding: 20px 10px
      line-height: 20px
      box-shadow: none
      border-bottom: 1px solid #f5f5f5
  .header
    flex-shrink: 1
</style>
