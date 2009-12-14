struct hsl {
    int hue; // 0..360
    float saturation; // 0..1
    float lightness; // 0..1
};

struct rgb {
    float red;
    float green;
    float blue;
};

void hsl2rgb(struct hsl source_color, struct rgb * destination_color);
