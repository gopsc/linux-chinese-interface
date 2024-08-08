<!--
 * @Author: 绪锋
 * @Date: 2024-06-21 08:37:24
 * @LastEditors: 绪锋
 * @LastEditTime: 2024-06-28 17:32:09
 * @FilePath: \note\src\views\AboutView.vue
 * @Description: 
-->
<template>
  <div class="home">
    <var-app-bar class="h-header" >
      <template #left>
        <var-button color="transparent" text-color="#fff" round text @click="handleBack()" >
          <var-icon name="chevron-left" :size="24" />
        </var-button>
      </template>
      <template #default>
        <div class="h-title">{{ menuInfo.name }}</div>
      </template>
    </var-app-bar>

    <var-breadcrumbs style="padding: 10px;">
      <var-breadcrumb>目录</var-breadcrumb>
      <var-breadcrumb v-for="item in nodeListAll" :key="item.id">{{item.name}}</var-breadcrumb>
    </var-breadcrumbs>
    <var-fab type="primary">
      <var-button type="danger" icon-container @click="handleDlete">
        <var-icon name="delete" />
      </var-button>
      <var-button type="primary" icon-container @click="handleSort">
        <var-icon name="menu-open" />
      </var-button>
      <var-button type="success" icon-container @click="handleEdit">
        <var-icon name="file-document-outline" />
      </var-button>
      <var-button type="warning" icon-container @click="handleAdd">
        <var-icon name="plus" />
      </var-button>
    </var-fab>

    <div class="content-wrapper">
      <div class="node-list">
        <template v-if="nodeInfo.sonList">
          <div
            v-for="item in nodeInfo.sonList"
            :key="item"
            class="item ripple-example-block var-elevation--2"
            v-ripple
            @click="handleToChild(item)"
          >{{item.name}}</div>
        </template>
        <div v-else>暂无子节点列表</div>
      </div>
      <div class="node-info">
        <var-divider description="节点信息" />
        <div class="title">{{nodeInfo.name}}</div>
        <div class="content">
          <var-input textarea v-model="nodeInfo.title" readonly :line="false"></var-input>
        </div>
      </div>
    </div>
    <!-- 新增节点 -->
    <var-dialog v-model:show="nodeShow" @confirm="handleConfirm">
      <template #title>新增节点</template>
      <var-form 
        ref="form" 
        scroll-to-error="start"
      >
        <var-space direction="column" :size="[14, 0]">
          <var-input
            placeholder="请输入节点名称"
            :rules="[v => !!v || '名称不能为空']"
            v-model="nodeInfoData.name"
          />
        </var-space>
        <var-space direction="column" :size="[14, 0]">
          <var-input
            textarea
            placeholder="请输入节点信息"
            :rules="[v => !!v || '内容不能为空']"
            v-model="nodeInfoData.title"
          />
        </var-space>
      </var-form>
    </var-dialog>

    <!-- 编辑节点信息 -->
    <var-dialog v-model:show="nodeInfoShow" @confirm="handleUpdata">
      <template #title>编辑节点信息</template>
      <var-form 
        ref="form" 
        scroll-to-error="start"
      >
        <var-space direction="column" :size="[14, 0]">
          <var-input
            placeholder="请输入节点名称"
            :rules="[v => !!v || '名称不能为空']"
            v-model="nodeInfoData.name"
          />
        </var-space>
        <var-space direction="column" :size="[14, 0]">
          <var-input
            textarea 
            placeholder="请输入节点信息"
            :rules="[v => !!v || '内容不能为空']"
            v-model="nodeInfoData.title"
          />
        </var-space>
      </var-form>
    </var-dialog>

    <!-- 节点树 -->
    <var-dialog v-model:show="nodeTreeDialog" @confirm="handleNodeSort">
      <template #title>将节点移动到</template>
      <div class="node-tree" v-if="nodeTreeList.length">
        <Tree :data="nodeTreeList" />
      </div>
      <div v-else>暂无数据</div>
    </var-dialog>

    <!-- 节点删除 -->
    <var-dialog
      title="提示"
      message="你要删除这个节点么？此操作不可恢复!"
      v-model:show="nodeDelShow"
      @confirm="handleDle"
    />
  </div>
</template>

<script setup>
import {useRouter} from 'vue-router'
import { useStore } from 'vuex'
import { onMounted, ref } from 'vue'
import { Snackbar } from '@varlet/ui'
import axios from 'axios'
import Tree from '../components/Tree.vue'

const store = useStore()
const router = useRouter()
const menuInfo = ref({})
const nodeListAll = ref([])
onMounted(() => {
  init()
})

const init = () => {
  menuInfo.value = store.state.nodeList[store.state.nodeList.length - 1]
  nodeListAll.value = store.state.nodeList
  handleQuery()
}

// 查询节点树
const nodeTreeList = ref([])
const bookTitleList = () => {
  axios({
    url: '/api/v1/bookTitleList/',
    method: 'post',
    headers: {
      'Content-Type': 'application/json'
    },
    params:{}
  }).then(res => {
    nodeTreeList.value = res.data
  })
}

// 查询节点详情
const nodeInfo = ref({})
const handleQuery = () => {
  nodeInfoData.value = {
    title: '',
    name: ''
  }
  axios({
    url: '/api/v1/queryBook/',
    method: 'post',
    headers: {
      'Content-Type': 'application/json'
    },
    params:{
      id: menuInfo.value.id
    }
  })
    .then(res => {
      menuInfo.value = {...menuInfo.value, ...res.data}
      nodeInfo.value = res.data
      nodeListAll.value[nodeListAll.value.length-1] = {
        ...nodeListAll.value[nodeListAll.value.length-1],
        ...res.data
      }
    })
}

// 移动节点排序
const nodeTreeDialog = ref(false)
const handleSort = () => {
  store.commit('setCurrentNode', {})
  nodeTreeDialog.value = true
  bookTitleList()
}
const handleNodeSort = () => {
  let pitem = store.state.currentNode
  let ids = []
  function filterId(data) {
    ids.push(data.id)
    if (data.sonList) {
      data.sonList.forEach(item => {
        filterId(item)
      })
    }
  }
  filterId(menuInfo.value)
  if (ids.indexOf(pitem.id) === -1) {
    axios({
    url: '/api/v1/updateBook/',
    method: 'post',
    headers: {
      'Content-Type': 'application/json'
    },
    params:{
      pid: pitem.id,
      id: menuInfo.value.id,
      name: menuInfo.value.name,
      title: menuInfo.value.title
    }
  })
    .then(res => {
      Snackbar.success('节点移动成功，即将返回首页重新组装数据')
      setTimeout(() => {
        router.push('/')
      }, 3000)
    })
  } else {
    Snackbar.error('禁止移动到子节点')
  }
}

// 编辑节点详情
const nodeInfoData = ref({
  title: '',
  name: ''
})
const handleUpdata = () => {
  axios({
    url: '/api/v1/updateBook/',
    method: 'post',
    headers: {
      'Content-Type': 'application/json'
    },
    params:{
      pid: menuInfo.value.pid,
      id: menuInfo.value.id,
      name: nodeInfoData.value.name,
      title: nodeInfoData.value.title
    }
  })
    .then(res => {
      handleQuery()
      Snackbar.success(res.data)
    })
}

// 编辑
const handleEdit = () => {
  nodeInfoShow.value = true
  nodeInfoData.value = {
    name: menuInfo.value.name,
    title: menuInfo.value.title,
  }
}

// 返回
const handleBack = () => {
  let nodeList = [...store.state.nodeList]
  nodeList.pop()
  store.commit('setNodeList', nodeList)
  if (nodeList.length === 0) {
    router.push('/')
  } else {
    init()
  }
}

// 子节点
const handleToChild = (item) => {
  let nodeList = [...store.state.nodeList, item]
  store.commit('setNodeList', nodeList)
  init()
}

const nodeShow = ref(false)
const nodeInfoShow = ref(false)

// 新增节点
const handleConfirm = () => {
  axios({
    url: '/api/v1/addBook/' ,
    method: 'post',
    headers: {
      'Content-Type': 'application/json'
    },
    params:{
      pid: menuInfo.value.id,
      name: nodeInfoData.value.name,
      title: nodeInfoData.value.title
    }
  })
    .then(res => {
      Snackbar.success(res.data)
      handleQuery()
    })
}

// 删除节点
const nodeDelShow = ref(false)
const handleDle = () => {
  axios({
    url: '/api/v1/deleteBook/' ,
    method: 'post',
    headers: {
      'Content-Type': 'application/json'
    },
    params:{
      id: menuInfo.value.id,
    }
  })
    .then(res => {
      Snackbar.success(res.data)
      handleBack()
    })
}
const handleDlete = () => {
  if (nodeInfo.value.sonList.length) {
    Snackbar.error('存在子节点，无法删除')
  } else {
    nodeDelShow.value = true
  }
}

const handleAdd = () => {
  nodeShow.value = true
  nodeInfoData.value = {
    name: '',
    title: ''
  }
}


</script>

<style lang="stylus" scoped>
.home
  position: relative
  width: 100vw
  height: 100vh
  display: flex
  flex-direction: column
  & >>> .h-header
    .var-app-bar__left
      width: 100%
    .var-app-bar__title
      justify-content: flex-start
    .h-title
      // width: 50%
      white-space: nowrap; /* 限制文本不换行 */
      overflow: hidden; /* 超出部分隐藏 */
      text-overflow: ellipsis; 
  .btn-plus
    position: absolute
    bottom: 100px
    right: 50px
    width: 50px
    height: 50px
    z-index 9
  .content-wrapper
    flex: 1
    display: flex
    flex-direction: column
    justify-content: flex-start
    overflow-y: scroll
    .node-list
      flex-shrink: 1
      height: 40%
      overflow-y: scroll
      .item
        padding: 20px 10px
        line-height: 20px
        box-shadow: none
        border-bottom: 1px solid #f5f5f5
    .node-info
      padding: 20px 10px
      .title
        padding-bottom: 20px
        font-size: 20px
        color: #303030
        line-height: 25px
      .content
        font-size: 14px
        line-height: 20px
        color: #616161
        word-wrap: break-word

.node-tree
  max-height: 300px
  overflow-y: auto
</style>