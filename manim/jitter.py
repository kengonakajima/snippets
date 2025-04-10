from manim import *

class JitterBufferAnimation(Scene):
    def construct(self):
        # ジッタバッファを表現する矩形
        buffer_rect = Rectangle(width=6, height=1, color=BLUE)
        buffer_label = Text("ジッタバッファ").next_to(buffer_rect, UP)

        # オーディオパケットを表現する矩形
        packet_width = 0.5
        packet_height = 0.8
        packet1 = Rectangle(width=packet_width, height=packet_height, color=RED).shift(LEFT*2)
        packet2 = Rectangle(width=packet_width, height=packet_height, color=RED).shift(LEFT)
        packet3 = Rectangle(width=packet_width, height=packet_height, color=RED)
        packet4 = Rectangle(width=packet_width, height=packet_height, color=RED).shift(RIGHT)
        packet5 = Rectangle(width=packet_width, height=packet_height, color=RED).shift(RIGHT*2)

        # 再生中のオーディオを表現する矢印
        arrow = Arrow(start=buffer_rect.get_left(), end=packet3.get_right(), color=GREEN)

        # アニメーションの構築
        self.play(Create(buffer_rect), Write(buffer_label))
        self.play(Create(packet1), Create(packet2), Create(packet3), Create(packet4), Create(packet5))
        self.play(Create(arrow))

        self.play(packet1.animate.shift(RIGHT*0.5), packet2.animate.shift(RIGHT*0.5), packet3.animate.shift(RIGHT*0.5),
                  packet4.animate.shift(RIGHT*0.5), packet5.animate.shift(RIGHT*0.5), arrow.animate.shift(RIGHT*0.5),
                  run_time=2, rate_func=linear)

        self.wait(1)

        self.play(packet1.animate.shift(RIGHT*0.5), packet2.animate.shift(RIGHT*0.5), packet3.animate.shift(RIGHT*0.5),
                  packet4.animate.shift(RIGHT*0.5), packet5.animate.shift(RIGHT*0.5), arrow.animate.shift(RIGHT*0.5),
                  run_time=2, rate_func=linear)

        self.wait(2)
