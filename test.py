from z3 import *
import random

# --- 1. 参数定义 ---
num_variables = 100
num_clauses = 150000
literals_per_clause = 3

# --- 2. 创建布尔变量 ---
variables = [Bool(f"v_{i}") for i in range(num_variables)]

# --- 3. 生成随机子句 ---
clauses = []
for _ in range(num_clauses):
    # 为每个子句随机选择3个不同的变量
    chosen_vars = random.sample(variables, literals_per_clause)
    
    # 对每个选中的变量，随机决定是否取非
    literals = [
        random.choice([var, Not(var)]) for var in chosen_vars
    ]
    
    # 创建一个子句（文字的析取）
    clause = Or(literals)
    clauses.append(clause)

# --- 4. 构建完整的CNF表达式 ---
# 将所有子句合取起来
full_formula = And(clauses)

# --- 5. 简化表达式 ---
# 注意：对于一个巨大且随机的CNF，simplify可能不会大幅减少其大小
# 因为随机生成的子句之间很少有逻辑冗余
simplified_formula = simplify(full_formula)


# --- 6. 打印结果 ---
print("="*50)
print(f"成功创建了一个包含 {num_variables} 个变量和 {num_clauses} 个子句的CNF表达式。")
print("="*50)

# 由于完整的表达式太长，我们只展示前5个子句作为示例
print("\n原始表达式的前5个子句示例：")
for i in range(5):
    print(clauses[i])

print("\n...")

# 同样，只展示简化后表达式的前几项
print("\n简化后表达式的示例部分：")
# Z3的simplify可能会改变表达式的结构，我们打印简化后的对象
# 为了可读性，我们只打印对象的一部分
simplified_children = simplified_formula.children()
limit = min(5, len(simplified_children))
for i in range(limit):
    print(simplified_children[i])
    
print("\n...")

print(f"\n原始表达式中的子句数量: {len(full_formula.children())}")
print(f"简化后表达式中的子句数量: {len(simplified_formula.children())}")