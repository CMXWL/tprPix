#version 330 core

//-- 片段着色器的 主输出：颜色
out vec4 FragColor;

//-- 从 顶点着色器 传来的 数据（名称和类型 都要相同）
in vec2 TexCoord;   //-- 每个pix 在 tecture 上的坐标 [0.0, 1.0] [left_bottom]

//-- sampler 系列 是 采样器数据类型。
//-- 现在，我们创建了 1个 纹理采样器
uniform sampler2D texture1;


void main()
{
    //-- 取得 texture 颜色 --
    vec4 texColor = texture(texture1, TexCoord);

    //-- 半透明度小于 0.05 的像素直接不渲染 --
    //-- 通过这个方法来处理大部分 “要么实心要么全透明” 的图元
    if( texColor.a < 0.05 ){
        discard;
    }

    vec3 addColor = vec3( 0.87, 0.42, 0.42 );

    texColor.rgb = texColor.rgb * addColor;
    

    //FragColor = vec4( 0.2, 0.6, 0.9, 1.0 );
    //FragColor = vec4( texColor.rgb, 0.2 ); //- 半透明
    //FragColor = vec4( texColor.rgb * 0.5, 1.0);
    FragColor = vec4( 0.87, 0.42, 0.42, 0.8); //- 实心颜色
    //FragColor = vec4(  texColor.rgb, 0.9 );


}
