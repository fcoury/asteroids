use raylib::drawing::RaylibDrawHandle;

pub fn rand_float(d: &mut RaylibDrawHandle<'_>, min: f32, max: f32) -> f32 {
    const SCALE: i32 = 1_000_000;
    let t = d.get_random_value::<i32>(0..=SCALE) as f32 / SCALE as f32;
    min + t * (max - min)
}
