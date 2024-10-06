## CSS Style for Polygon Class

update on: 2024-10-05

参考：CSS_Standard.md

Polygon Shape 的绘制风格 CSS 如下：

    .polygon {
        attr1: value1;  // 属性名: 属性值;
        attr2: value2;
        ...
    }

下面的属性定义了多边形边界的绘制：

- border-color: 边框颜色

- border-style: 边框线型。取值如下，当前仅支持: none|dashed|solid

    none|hidden|inherit|dashed|solid|double|inset|outset|ridge|groove

- border-width: 边框线宽

上面 3 个属性可以合为下面的一个：

- border: border-width border-style border-color

下面的属性定义了多边形的内部填充绘制：

- fill-opacity: 0; // 0: 全透明, 1 部透明

- fill-style: solid

- fill-color: 填充颜色

上面 3 个属性可以合为下面的一个：

- fill: fill-opacity fill-style fill-color
  
TODO:

ID 选择器

#1 {

}

#2 {

}

通用选择器
* {
}

