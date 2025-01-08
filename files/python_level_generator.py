import random

def generate_maze_with_edges_fixed(rows, cols):
    # Create a matrix with walls on the edges
    maze = [['#' if x == 0 or y == 0 or x == rows-1 or y == cols-1 else '#' for y in range(cols)] for x in range(rows)]

    # Function to carve paths using a stack (iteratively)
    def carve_passages_stack(start_x, start_y):
        stack = [(start_x, start_y)]
        directions = [(0, 1), (1, 0), (0, -1), (-1, 0)]

        while stack:
            cx, cy = stack[-1]
            maze[cx][cy] = ' '  # Make current position walkable
            valid_neighbors = []

            for dx, dy in directions:
                nx, ny = cx + dx * 2, cy + dy * 2
                if 0 < nx < rows-1 and 0 < ny < cols-1 and maze[nx][ny] == '#':
                    valid_neighbors.append((nx, ny, cx + dx, cy + dy))

            if valid_neighbors:
                nx, ny, mx, my = random.choice(valid_neighbors)
                maze[mx][my] = ' '
                stack.append((nx, ny))
            else:
                stack.pop()

    # Carve the path starting from position (1,1)
    carve_passages_stack(1, 1)

    # Function to check if there's a path from the start to a target position
    def is_path_possible(start_x, start_y, target_x, target_y):
        visited = [[False] * cols for _ in range(rows)]
        stack = [(start_x, start_y)]
        directions = [(0, 1), (1, 0), (0, -1), (-1, 0)]

        while stack:
            cx, cy = stack.pop()
            if (cx, cy) == (target_x, target_y):
                return True
            if not visited[cx][cy]:
                visited[cx][cy] = True
                for dx, dy in directions:
                    nx, ny = cx + dx, cy + dy
                    if 0 < nx < rows-1 and 0 < ny < cols-1 and not visited[nx][ny] and maze[nx][ny] == ' ':
                        stack.append((nx, ny))
        return False

    # Randomize positions for '@' (player), 'X' (chest), and '&' (key)
    player_x, player_y = random.randint(2, rows-3), random.randint(2, cols-3)
    chest_x, chest_y = random.randint(2, rows-3), random.randint(2, cols-3)
    key_x, key_y = random.randint(2, rows-3), random.randint(2, cols-3)

    # Ensure there's a path between player, chest, and key by adjusting positions
    while not (is_path_possible(player_x, player_y, chest_x, chest_y) and is_path_possible(player_x, player_y, key_x, key_y)):
        player_x, player_y = random.randint(2, rows-3), random.randint(2, cols-3)
        chest_x, chest_y = random.randint(2, rows-3), random.randint(2, cols-3)
        key_x, key_y = random.randint(2, rows-3), random.randint(2, cols-3)

    # Place player, chest, and key in the maze
    maze[player_x][player_y] = '@'  # Player's starting position
    maze[chest_x][chest_y] = 'X'  # Chest at the end position
    maze[key_x][key_y] = '&'  # Key

    # Add between 3 and 6 portals ('%')
    portal_count = random.randint(3, 6)  # Random number of portals between 3 and 6
    portals_added = 0
    while portals_added < portal_count:
        x, y = random.randint(2, rows-3), random.randint(2, cols-3)
        if maze[x][y] == ' ':
            maze[x][y] = '%'  # Place portal
            portals_added += 1

    # Add coins 'C' in walkable positions, clustering them
    coin_count = (random.randint(0, 4) + 3) * (rows // 10) * (cols // 10)

    coins_added = 0
    coin_positions = []
    while coins_added < coin_count:
        x, y = random.randint(2, rows-3), random.randint(2, cols-3)
        if maze[x][y] == ' ':
            # Place the first coin in the cluster
            maze[x][y] = 'C'
            coin_positions.append((x, y))
            coins_added += 1
            
            # Now, place coins around this position to form a cluster
            directions = [(0, 1), (1, 0), (0, -1), (-1, 0)]  # Up, down, left, right
            for dx, dy in directions:
                nx, ny = x + dx, y + dy
                if 0 < nx < rows-1 and 0 < ny < cols-1 and maze[nx][ny] == ' ':
                    maze[nx][ny] = 'C'
                    coin_positions.append((nx, ny))
                    coins_added += 1
                    if coins_added >= coin_count:
                        break

    # Return the maze as text
    return '\n'.join(''.join(row) for row in maze)

# Generate a MxN maze
maze_MxN = generate_maze_with_edges_fixed(20, 20)
print(maze_MxN)
