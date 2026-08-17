use raylib::prelude::*;

use crate::utils::rand_float;

mod utils;

const SCREEN_WIDTH: i32 = 800;
const SCREEN_HEIGHT: i32 = 450;
const STARTING_ASTEROIDS: i32 = 4;
const ROT_SPEED: f32 = 5.0;
const SHIP_ACCELERATION: f32 = 200.0;

struct Ship {
    pos: Vector2,
    velocity: Vector2,
    rot: f32,
}

impl Default for Ship {
    fn default() -> Self {
        Self {
            pos: Vector2::new(SCREEN_WIDTH as f32 / 2.0, SCREEN_HEIGHT as f32 / 2.0),
            velocity: Vector2::new(0.0, 0.0),
            rot: 0.0,
        }
    }
}

enum AsteroidSize {
    Large,
    Medium,
    Small,
}

struct Asteroid {
    pos: Vector2,
    velocity: Vector2,
    rot: f32,
    rot_speed: f32,
    size: AsteroidSize,
    active: bool,
}

struct Bullet {
    pos: Vector2,
    velocity: Vector2,
    rot: f32,
    active: bool,
}

#[derive(Default)]
enum GameState {
    #[default]
    Ready,
    Playing,
    GameOver,
}

#[derive(Default)]
struct Game {
    ship: Ship,
    asteroids: Vec<Asteroid>,
    bullets: Vec<Bullet>,
    wave: i32,
    state: GameState,
}

impl Game {
    fn update(&mut self, d: &mut RaylibDrawHandle<'_>) {
        let dt = d.get_frame_time();
        let forward = Vector2::new(self.ship.rot.cos(), self.ship.rot.sin());

        match self.state {
            GameState::GameOver | GameState::Ready if d.is_key_pressed(KeyboardKey::KEY_ENTER) => {
                self.spawn_wave(d)
            },
            GameState::Playing if d.is_key_pressed(KeyboardKey::KEY_ENTER) => {
                self.fire()
            },
            GameState::Ready => return,
            _ => (),
        };

        if d.is_key_down(KeyboardKey::KEY_LEFT) || d.is_key_down(KeyboardKey::KEY_H) {
            self.ship.rot -= ROT_SPEED * dt;
        }

        if d.is_key_down(KeyboardKey::KEY_RIGHT) || d.is_key_down(KeyboardKey::KEY_L) {
            self.ship.rot += ROT_SPEED * dt;
        }

        if d.is_key_down(KeyboardKey::KEY_DOWN) || d.is_key_down(KeyboardKey::KEY_J) {
            let acceleration = forward.scale(SHIP_ACCELERATION);
            self.ship.velocity = self.ship.velocity + acceleration.scale(dt);
        }
    }

    fn draw(&mut self, d: &mut RaylibDrawHandle<'_>) {
        d.clear_background(Color::BLACK);

        match self.state {
            GameState::Ready => {
                d.draw_text("Press ENTER to start", SCREEN_WIDTH / 2 - 100, SCREEN_HEIGHT / 2, 20, Color::WHITE);
            },
            GameState::Playing => {
                // draw ship
                let ship_points = [
                    Vector2::new(0.0, -10.0),
                    Vector2::new(-5.0, 5.0),
                    Vector2::new(5.0, 5.0),
                ];
                let ship_rotated: Vec<Vector2> = ship_points.iter().map(|p| {
                    let x = p.x * self.ship.rot.cos() - p.y * self.ship.rot.sin();
                    let y = p.x * self.ship.rot.sin() + p.y * self.ship.rot.cos();
                    Vector2::new(x + self.ship.pos.x, y + self.ship.pos.y)
                }).collect();
                d.draw_triangle_lines(ship_rotated[0], ship_rotated[1], ship_rotated[2], Color::WHITE);
            },
            GameState::GameOver => {
                d.draw_text("Game Over! Press ENTER to restart", SCREEN_WIDTH / 2 - 150, SCREEN_HEIGHT / 2, 20, Color::WHITE);
            },
        }
    }

    fn spawn_wave(&mut self, d: &mut RaylibDrawHandle<'_>) {
        self.wave += 1;

        let count = STARTING_ASTEROIDS + (self.wave - 1);
        for i in 0..count {
            let pos = if d.get_random_value::<i32>(0..=1) == 0 {
                Vector2::new(
                    if d.get_random_value::<i32>(0..=1) == 0 {
                        0.0
                    } else {
                        SCREEN_HEIGHT as f32
                    },
                    rand_float(d, 0.0, SCREEN_WIDTH as f32)
                )
            } else {
                Vector2::new(
                    rand_float(d, 0.0, SCREEN_HEIGHT as f32),
                    if d.get_random_value::<i32>(0..=1) == 0 {
                        0.0
                    } else {
                        SCREEN_WIDTH as f32
                    },
                )
            };
            self.spawn_asteroid(d, pos, AsteroidSize::Large);
        }
    }

    fn spawn_asteroid(&self, d: &mut RaylibDrawHandle<'_>, _pos: Vector2, _size: AsteroidSize) {
        // random float from 0 to 2pi
        // let angle = rand_float(d, 0.0, std::f32::consts::PI * 2.0);
        todo!()
    }

    fn fire(&self) {
        todo!()
    }
}

fn main() {
    let (mut rl, thread) = raylib::init()
        .size(SCREEN_WIDTH, SCREEN_HEIGHT)
        .title("Asteroidz")
        .build();

    rl.set_target_fps(60);

    let mut g = Game::default();

    while !rl.window_should_close() {
        let mut d = rl.begin_drawing(&thread);
        g.update(&mut d);
        g.draw(&mut d);
    }
}

