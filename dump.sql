CREATE TABLE `users` (
  `id` int PRIMARY KEY NOT NULL AUTO_INCREMENT,
  `email` varchar(255) NOT NULL,
  `password` varchar(255) NOT NULL,
  `first_name` varchar(255) NOT NULL,
  `last_name` varchar(255) NOT NULL,
  `gender` ENUM ('male', 'female') NOT NULL,
  `phone` varchar(255) NOT NULL,
  `birth_date` date NOT NULL,
  `passport_serial` varchar(255) NOT NULL,
  `passport_number` varchar(255) NOT NULL
);

CREATE TABLE `wallets` (
  `id` int PRIMARY KEY NOT NULL AUTO_INCREMENT,
  `user_id` int UNIQUE NOT NULL,
  `balance` double NOT NULL DEFAULT 0
);

CREATE TABLE `trains` (
  `id` int PRIMARY KEY NOT NULL AUTO_INCREMENT,
  `name` varchar(255) NOT NULL,
  `quantity_carriage` int NOT NULL,
  `quantity_seats` int NOT NULL,
  `rate_per_km` double NOT NULL,
  `avg_speed` double NOT NULL
);

CREATE TABLE `schedules` (
  `id` int PRIMARY KEY NOT NULL AUTO_INCREMENT,
  `train_id` int NOT NULL,
  `departure_city` varchar(255) NOT NULL,
  `arrival_city` varchar(255) NOT NULL,
  `departure_datetime` datetime NOT NULL,
  `arrival_datetime` datetime NOT NULL,
  `distance` double NOT NULL,
  `is_finished` bool DEFAULT false
);

CREATE TABLE `bookings` (
  `id` int PRIMARY KEY NOT NULL AUTO_INCREMENT,
  `user_id` int NOT NULL,
  `schedule_id` int NOT NULL,
  `price` double
);

CREATE TABLE `feedbacks` (
  `id` int PRIMARY KEY NOT NULL AUTO_INCREMENT,
  `user_id` int NOT NULL,
  `body` text
);

CREATE TABLE `payments` (
  `id` int PRIMARY KEY NOT NULL AUTO_INCREMENT,
  `booking_id` int NOT NULL,
  `amount` double
);

ALTER TABLE `wallets` ADD FOREIGN KEY (`user_id`) REFERENCES `users` (`id`);

ALTER TABLE `schedules` ADD FOREIGN KEY (`train_id`) REFERENCES `trains` (`id`);

ALTER TABLE `bookings` ADD FOREIGN KEY (`user_id`) REFERENCES `users` (`id`);

ALTER TABLE `bookings` ADD FOREIGN KEY (`schedule_id`) REFERENCES `schedules` (`id`);

ALTER TABLE `feedbacks` ADD FOREIGN KEY (`user_id`) REFERENCES `users` (`id`);

ALTER TABLE `payments` ADD FOREIGN KEY (`booking_id`) REFERENCES `bookings` (`id`);

