/**
 * @file Render/Utils.ixx
 * @author Cheyne Xie
 * @brief 一些渲染相关的公用函数
 * @version 1.0
 * @date 2026-06-25
 * 
 */

module;
#include <glad/glad.h>
export module CEngine.Render:Utils;

namespace CEngine {
    /**
     * @brief 获取一个空的 VAO（无顶点缓冲）
     * @remark Core Profile 下 glDrawArrays 需要绑定 VAO。全屏三角形（由 gl_VertexID 生成顶点）
     *         无需任何顶点属性，故复用此空 VAO。惰性创建，引擎生命周期内不释放。
     */
    export unsigned int GetEmptyVAO() {
        static unsigned int vao = 0;
        if (vao == 0) glGenVertexArrays(1, &vao);
        return vao;
    }
}