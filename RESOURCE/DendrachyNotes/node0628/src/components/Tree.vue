
<!--
 * @Author: 绪锋
 * @Date: 2024-06-20 19:34:13
 * @LastEditors: 绪锋
 * @LastEditTime: 2024-06-28 16:58:23
 * @FilePath: \note\src\components\Tree.vue
 * @Description: 树结构
-->
<template>
    <div class="tree">
        <div class="tree-item" v-for="item in data" :key="item.id">
            <div :class="['tree-item-name', currentNodeId === item.id && 'item-active']" @click="handleCheck(item)">{{item.name}}</div>
            <Tree :data="item.children" v-if="item.children && item.children.length > 0"></Tree>
        </div>
    </div>
</template>

<script setup>
import { computed, defineComponent } from 'vue'
import { useStore } from 'vuex'

const store = useStore()

defineComponent({
    name: 'Tree'
})

defineProps({
    data: {
        type: Array,
        default: () => []
    }
})


const currentNodeId = computed(() => store.state.currentNode?.id)

const handleCheck = (item) => {
    store.commit('setCurrentNode', item)
    console.log(store.state.currentNode, item)
}
</script>

<style lang="stylus" scoped>
.tree
    padding-left 15px
    .tree-item
        .tree-item-name
            padding 5px 10px
            flex 1
            font-size 14px
            line-height: 30px
            color #333
            white-space nowrap
            overflow hidden
            text-overflow ellipsis
        .item-active
            color: #fff
            background: #3A7AFE
</style>
