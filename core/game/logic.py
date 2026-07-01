from core.config.settings import permitted_classes, game_objects


class GameLogic:
    def __init__(self):
        self.object_order = permitted_classes
        self.reset_game()
        self.has_connected = False

    def connect_hardware(self):
        self.reset_game()
        self.has_connected = True

    def reset_game(self):
        self.current_level_index = 0
        self.hints_requested = 0
        self.total_score = 0
        self.is_game_active = True
        self.has_started = False

    def start_game(self):
        self.reset_game()
        self.has_started = True

    def get_current_object(self):

        if self.current_level_index >= len(self.object_order):
            self.is_game_active = False
            return None

        yolo_class = self.object_order[self.current_level_index]
        object_data = game_objects[yolo_class]

        return {
            "yolo_class": yolo_class,
            "display_name": object_data["display_name"],
            "hints": object_data["hints"],
        }

    def request_hint(self):
        current_object = self.get_current_object()
        if not current_object or not self.is_game_active:
            return "Fim de Jogo"

        if self.hints_requested < len(current_object["hints"]):
            hint_text = current_object["hints"][self.hints_requested]
            self.hints_requested += 1
            return hint_text
        else:
            return "Sem dicas!"

    def calculate_current_score(self):
        if self.hints_requested == 1:
            return 100
        elif self.hints_requested == 2:
            return 75
        elif self.hints_requested == 3:
            return 50
        elif self.hints_requested >= 4:
            return 25
        return 0

    def advance_object(self, is_correct):
        if is_correct:
            self.total_score += self.calculate_current_score()

        self.current_level_index += 1
        self.hints_requested = 0
